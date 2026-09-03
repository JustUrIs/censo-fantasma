/* ═══════════════════════════════════════════════════════════════════════════
   CENSO FANTASMA — firmware ESP32
   INDEC · Dirección Nacional de Teledetección Habitacional

   Placa:  NodeMCU ESP32-WROOM  →  en Arduino IDE elegir "ESP32 Dev Module"
   Salida: Serial a 921600 baudios, CSV, consumido por el navegador con Web Serial

   ───────────────────────────────────────────────────────────────────────────
   CÓMO USARLO: cambiá UNA sola línea, la de abajo, y flasheá cada placa.

       ROLE 1 = SNIFFER   → censo de dispositivos.      1 placa.  No necesita las otras.
       ROLE 2 = CSI_RX    → receptor de CSI (por USB a la laptop).
       ROLE 3 = CSI_TX    → emisor. Solo necesita corriente (power bank).

   Para el censo de dispositivos:      1 placa con ROLE 1.
   Para todo lo demás (CSI):           1 placa ROLE 3 + 1 placa ROLE 2.
   ═══════════════════════════════════════════════════════════════════════════ */

#define ROLE 1        // ← CAMBIÁ ESTE NÚMERO

// ───────────────────────────────────────────────────────────────────────────

#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_wifi.h"

#define BAUD        921600
#define AP_SSID     "CENSO_FANTASMA"
#define AP_PASS     "teledeteccion"
#define AP_CHANNEL  6
#define UDP_PORT    17622          // Ley 17.622

// ═══════════════════════════════════════════════════════════════════════════
// ROLE 1 — SNIFFER: censo de dispositivos
//
// Modo promiscuo: se escucha todo el tráfico 802.11 del aire. Cada teléfono
// emite probe requests buscando redes conocidas, aunque no esté conectado a
// nada. Se cuentan emisores únicos.
//
// PRIVACIDAD — decisión de diseño, no accidente:
// la dirección MAC se hashea apenas se lee. No se guarda, no se imprime y no
// sale de la placa en claro. Solo salen CANTIDADES.
// ═══════════════════════════════════════════════════════════════════════════
#if ROLE == 1

#define TABLA      1024            // ranuras de la tabla hash (potencia de 2)
#define VENTANA_MS 60000UL         // un dispositivo "sigue presente" 60 s
#define CANALES    13
#define SALTO_MS   220             // permanencia por canal

struct Ranura { uint32_t h; uint32_t visto; };
static Ranura tabla[TABLA];
static volatile uint32_t paquetes = 0;
static uint8_t canal = 1;
static uint32_t tSalto = 0, tReporte = 0;

// FNV-1a de 32 bits. Irreversible a efectos prácticos para lo que hacemos:
// nunca se compara contra una lista, solo se cuenta.
static inline uint32_t hashMac(const uint8_t *m){
  uint32_t h = 2166136261u;
  for (int i = 0; i < 6; i++) { h ^= m[i]; h *= 16777619u; }
  return h ? h : 1;               // 0 marca ranura vacía
}

static void registrar(const uint8_t *mac){
  uint32_t h = hashMac(mac);
  uint32_t i = h & (TABLA - 1);
  for (int k = 0; k < 32; k++){    // sondeo lineal acotado
    uint32_t j = (i + k) & (TABLA - 1);
    if (tabla[j].h == h || tabla[j].h == 0){
      tabla[j].h = h;
      tabla[j].visto = millis();
      return;
    }
  }
}

static void snifferCb(void *buf, wifi_promiscuous_pkt_type_t type){
  if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;
  const wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t *)buf;
  if (p->rx_ctrl.sig_len < 24) return;             // sin cabecera completa
  registrar(p->payload + 10);                      // addr2 = emisor
  paquetes++;
}

void setup(){
  Serial.begin(BAUD);
  delay(300);
  memset((void*)tabla, 0, sizeof(tabla));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  wifi_promiscuous_filter_t f = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
  };
  esp_wifi_set_promiscuous_filter(&f);
  esp_wifi_set_promiscuous_rx_cb(&snifferCb);
  esp_wifi_set_channel(canal, WIFI_SECOND_CHAN_NONE);

  Serial.println("#ROLE,SNIFFER");
  Serial.println("#FMT,S,unicos,paqSeg,canal,totalPaq");
}

void loop(){
  uint32_t ahora = millis();

  if (ahora - tSalto >= SALTO_MS){                 // barrido de canales
    tSalto = ahora;
    canal = (canal % CANALES) + 1;
    esp_wifi_set_channel(canal, WIFI_SECOND_CHAN_NONE);
  }

  if (ahora - tReporte >= 500){
    uint32_t dt = ahora - tReporte;
    tReporte = ahora;

    uint32_t vivos = 0;
    for (uint32_t i = 0; i < TABLA; i++){
      if (!tabla[i].h) continue;
      if (ahora - tabla[i].visto > VENTANA_MS) tabla[i].h = 0;   // expira
      else vivos++;
    }
    uint32_t pq = paquetes; paquetes = 0;

    Serial.printf("S,%lu,%lu,%u,%lu\n",
                  (unsigned long)vivos,
                  (unsigned long)(pq * 1000UL / (dt ? dt : 1)),
                  canal,
                  (unsigned long)pq);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// ROLE 2 — CSI_RX: receptor de Channel State Information
//
// Se conecta al AP que levanta la placa emisora y habilita el callback de CSI.
// Cada paquete recibido trae la respuesta del canal medida en 64 subportadoras.
// Se imprime la amplitud de cada una: sqrt(re² + im²).
// ═══════════════════════════════════════════════════════════════════════════
#elif ROLE == 2

#define MAX_SUB     64
#define MIN_PERIODO 18            // ms entre reportes → ~55 Hz, no satura el serial

static volatile uint32_t tUltimo = 0;
static volatile uint32_t recibidos = 0;
static char linea[600];

static void csiCb(void *ctx, wifi_csi_info_t *info){
  if (!info || !info->buf) return;
  recibidos++;

  uint32_t ahora = millis();
  if (ahora - tUltimo < MIN_PERIODO) return;       // diezmado
  tUltimo = ahora;

  const int8_t *b = info->buf;
  int pares = info->len / 2;
  if (pares > MAX_SUB) pares = MAX_SUB;

  int n = snprintf(linea, sizeof(linea), "C,%d,%d", info->rx_ctrl.rssi, pares);
  for (int i = 0; i < pares; i++){
    int im = b[i*2], re = b[i*2 + 1];
    int amp = (int)(sqrtf((float)(re*re + im*im)) + 0.5f);
    n += snprintf(linea + n, sizeof(linea) - n, ",%d", amp);
    if (n >= (int)sizeof(linea) - 8) break;
  }
  linea[n++] = '\n'; linea[n] = 0;
  Serial.write((const uint8_t*)linea, n);
}

WiFiUDP udp;
static uint32_t tPing = 0, tEstado = 0;

void setup(){
  Serial.begin(BAUD);
  delay(300);
  Serial.println("#ROLE,CSI_RX");
  Serial.println("#FMT,C,rssi,n,amp0..ampN");

  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  Serial.println("#INFO,conectando al emisor...");
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) delay(200);

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("#ERROR,no encuentro el emisor. Encendé la placa ROLE 3 y reiniciá esta.");
  } else {
    Serial.printf("#INFO,enlace establecido, canal %d\n", WiFi.channel());
  }

  udp.begin(UDP_PORT);

  wifi_csi_config_t cfg = {};
  cfg.lltf_en           = true;
  cfg.htltf_en          = true;
  cfg.stbc_htltf2_en    = true;
  cfg.ltf_merge_en      = true;
  cfg.channel_filter_en = true;
  cfg.manu_scale        = false;
  esp_wifi_set_csi_config(&cfg);
  esp_wifi_set_csi_rx_cb(&csiCb, NULL);
  esp_wifi_set_csi(true);
}

void loop(){
  uint32_t ahora = millis();

  // Provocamos tráfico: cada paquete que mandamos genera un ACK entrante,
  // y cada paquete entrante trae una medición de CSI.
  if (ahora - tPing >= 8){
    tPing = ahora;
    udp.beginPacket(WiFi.gatewayIP(), UDP_PORT);
    udp.write((const uint8_t*)"x", 1);
    udp.endPacket();
  }

  if (ahora - tEstado >= 2000){
    tEstado = ahora;
    Serial.printf("#STAT,paq,%lu,rssi,%d,link,%d\n",
                  (unsigned long)recibidos, WiFi.RSSI(),
                  WiFi.status() == WL_CONNECTED);
    recibidos = 0;
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// ROLE 3 — CSI_TX: emisor
//
// Levanta el punto de acceso y emite a ritmo fijo. NO necesita datos por USB:
// solo corriente. Enchufada a un power bank se puede dejar del otro lado de una
// pared, o dársela a alguien del público para que camine con ella.
// ═══════════════════════════════════════════════════════════════════════════
#elif ROLE == 3

WiFiUDP udp;
static uint32_t tTx = 0, tLed = 0;
static bool led = false;

void setup(){
  Serial.begin(BAUD);
  delay(300);
  pinMode(2, OUTPUT);                              // LED de la NodeMCU

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL);
  esp_wifi_set_max_tx_power(78);                   // potencia máxima permitida
  udp.begin(UDP_PORT);

  Serial.println("#ROLE,CSI_TX");
  Serial.printf("#INFO,AP %s en canal %d — solo necesita corriente\n",
                AP_SSID, AP_CHANNEL);
}

void loop(){
  uint32_t ahora = millis();

  if (ahora - tTx >= 5){                           // ~200 paquetes por segundo
    tTx = ahora;
    udp.beginPacket(IPAddress(192,168,4,255), UDP_PORT);
    udp.write((const uint8_t*)"CENSO", 5);
    udp.endPacket();
  }

  if (ahora - tLed >= 500){                        // late = está emitiendo
    tLed = ahora;
    led = !led;
    digitalWrite(2, led);
  }
}

#else
  #error "ROLE tiene que ser 1 (SNIFFER), 2 (CSI_RX) o 3 (CSI_TX)"
#endif
