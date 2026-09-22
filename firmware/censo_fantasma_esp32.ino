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
       ROLE 4 = RADAR     → puente al LD2450 (sigue hasta 3 personas).

   Para el censo de dispositivos:      1 placa con ROLE 1.
   Para todo lo demás (CSI):           1 placa ROLE 3 + 1 placa ROLE 2.
   ═══════════════════════════════════════════════════════════════════════════ */

#define ROLE 1        // ← CAMBIÁ ESTE NÚMERO

// ───────────────────────────────────────────────────────────────────────────

#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_wifi.h"
#include <math.h>

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

#define TABLA      2048            // ranuras de la tabla hash (potencia de 2)
#define VENTANA_MS 25000UL         // un dispositivo "sigue presente" 25 s

/* UMBRAL DE CERCANIA -- el arreglo del contador que crecia para siempre.
   Sin filtro se cuentan telefonos del edificio entero, de la vereda y del bar
   de al lado; y como los telefonos modernos cambian de nombre cada rato, el
   mismo aparato entra varias veces. Por eso nunca bajaba.

   Filtrando por potencia de senal contamos solo lo que esta EN LA SALA: el
   numero se estabiliza y ademas significa algo.

     -55  muy cerca, pocos metros
     -65  la sala            <- por defecto
     -75  la sala y lo pegado
     -95  todo (comportamiento viejo)                                        */
#define RSSI_MIN   -65
#define CANALES    13
#define SALTO_MS   220             // permanencia por canal

struct Ranura { uint32_t h; uint32_t visto; };
static Ranura tabla[TABLA];
static volatile uint32_t paquetes = 0;
static volatile uint32_t descartados = 0;   // vistos, pero fuera de la sala
static uint8_t canal = 1;
static uint32_t tSalto = 0, tReporte = 0;

// FNV-1a de 32 bits. Irreversible a efectos prácticos para lo que hacemos:
// nunca se compara contra una lista, solo se cuenta.
static inline uint32_t hashMac(const uint8_t *m){
  uint32_t h = 2166136261u;
  for (int i = 0; i < 6; i++) { h ^= m[i]; h *= 16777619u; }
  return h ? h : 1;               // 0 marca ranura vacía
}

static void registrar(const uint8_t *mac, int8_t rssi){
  uint32_t h = hashMac(mac);
  uint32_t i = h & (TABLA - 1);
  for (int k = 0; k < 32; k++){    // sondeo lineal acotado
    uint32_t j = (i + k) & (TABLA - 1);
    if (tabla[j].h == h){           // ya lo conociamos: solo refrescar
      tabla[j].visto = millis();
      return;
    }
    if (tabla[j].h == 0){           // aparato NUEVO
      tabla[j].h = h;
      tabla[j].visto = millis();
      /* Se avisa el descubrimiento para poder dibujarlo en pantalla.
         Sale el hash, nunca la direccion: es irreversible y no se guarda.    */
      Serial.printf("D,%08lx,%d\n", (unsigned long)h, (int)rssi);
      return;
    }
  }
}

static void snifferCb(void *buf, wifi_promiscuous_pkt_type_t type){
  if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;
  const wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t *)buf;
  if (p->rx_ctrl.sig_len < 24) return;             // sin cabecera completa
  paquetes++;
  if (p->rx_ctrl.rssi < RSSI_MIN) { descartados++; return; }   // demasiado lejos
  registrar(p->payload + 10, p->rx_ctrl.rssi);     // addr2 = emisor
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
  Serial.println("#FMT,S,cerca,paqSeg,canal,lejanos,rssiMin | D,hash,rssi = aparato nuevo");
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
    uint32_t pq = paquetes;    paquetes = 0;
    uint32_t lj = descartados; descartados = 0;

    Serial.printf("S,%lu,%lu,%u,%lu,%d\n",
                  (unsigned long)vivos,
                  (unsigned long)(pq * 1000UL / (dt ? dt : 1)),
                  canal,
                  (unsigned long)lj,
                  RSSI_MIN);
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
#define MIN_PERIODO 25            // ms entre reportes -> ~40 Hz (I/Q pesa el doble)

static volatile uint32_t tUltimo = 0;
static volatile uint32_t recibidos = 0;
static char linea[1200];

static void csiCb(void *ctx, wifi_csi_info_t *info){
  if (!info || !info->buf) return;
  recibidos++;

  uint32_t ahora = millis();
  if (ahora - tUltimo < MIN_PERIODO) return;       // diezmado
  tUltimo = ahora;

  const int8_t *b = info->buf;
  int pares = info->len / 2;
  if (pares > MAX_SUB) pares = MAX_SUB;

  /* Se manda I y Q crudos, no la magnitud.
     Antes calculaba sqrt(re^2+im^2) aca y tiraba la fase. La fase es donde vive
     la mayor parte de la informacion de movimiento: un cuerpo que se desplaza un
     centimetro corre la fase muchisimo antes de cambiar la amplitud. Con solo
     amplitud, caminar apenas se separaba 1,5 desvios del ruido.               */
  int n = snprintf(linea, sizeof(linea), "Q,%d,%d", info->rx_ctrl.rssi, pares);
  for (int i = 0; i < pares; i++){
    n += snprintf(linea + n, sizeof(linea) - n, ",%d,%d", (int)b[i*2], (int)b[i*2 + 1]);
    if (n >= (int)sizeof(linea) - 10) break;
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
  Serial.println("#FMT,Q,rssi,n,im0,re0,im1,re1,...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  Serial.println("#INFO,conectando al emisor...");
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) delay(200);

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("#ERROR,no encuentro el emisor todavia. Sigo buscando solo.");
  } else {
    Serial.printf("#INFO,enlace establecido, canal %d\n", WiFi.channel());
  }

  udp.begin(UDP_PORT);

  csiActivar();
}

/* En el core 3.x, activar CSI antes de asociarse no queda aplicado: la pila
   lo reinicia al conectar. Hay que volver a pedirlo CADA vez que el enlace se
   establece. Este era el motivo de "enlace: si" con cero tramas.            */
void csiActivar(){
  wifi_csi_config_t cfg = {};
  cfg.lltf_en           = true;
  cfg.htltf_en          = true;
  cfg.stbc_htltf2_en    = true;
  cfg.ltf_merge_en      = true;
  cfg.channel_filter_en = true;
  cfg.manu_scale        = false;
  esp_wifi_set_csi_config(&cfg);
  esp_wifi_set_csi_rx_cb(&csiCb, NULL);
  esp_err_t e1 = esp_wifi_set_csi_config(&cfg);
  esp_err_t e2 = esp_wifi_set_csi(true);
  Serial.printf("#INFO,csi config=%d activar=%d\n", (int)e1, (int)e2);
}

void loop(){
  uint32_t ahora = millis();

  /* Reconexion. Antes, si la receptora arrancaba antes que la emisora, buscaba
     20 segundos en setup() y se rendia para siempre: habia que apretar RESET.
     Ahora sigue buscando sola, y tambien se recupera si el enlace se corta.     */
  static uint32_t tRecon = 0;
  static bool conectado = false;
  bool hay = WiFi.status() == WL_CONNECTED;
  if (hay != conectado){
    conectado = hay;
    if (conectado){
      Serial.printf("#INFO,enlace establecido, canal %d\n", WiFi.channel());
      csiActivar();                      // re-aplicar: sin esto no llega CSI
    }
    else           Serial.println("#INFO,enlace perdido, reintentando");
  }
  if (!conectado){
    if (ahora - tRecon >= 3000){
      tRecon = ahora;
      WiFi.disconnect();
      WiFi.begin(AP_SSID, AP_PASS);
      Serial.println("#INFO,buscando al emisor CENSO_FANTASMA...");
    }
    return;
  }

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
static uint32_t tTx = 0, tLed = 0, ecos = 0, tInf = 0;
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

  /* ECO. Esto es lo que faltaba para que hubiera CSI de verdad.
     El CSI solo se genera con paquetes RECIBIDOS. La receptora mandaba pings
     al vacio y nadie le contestaba, asi que lo unico que le llegaba eran las
     balizas de la red: 2 o 3 por segundo. Devolviendo cada ping, cada uno de
     esos pings pasa a ser una medicion del canal.                            */
  int sz = udp.parsePacket();
  if (sz > 0){
    IPAddress quien = udp.remoteIP();
    uint16_t  porto = udp.remotePort();
    uint8_t   basura[16];
    udp.read(basura, sz > 16 ? 16 : sz);
    udp.beginPacket(quien, porto);
    udp.write((const uint8_t*)"R", 1);
    udp.endPacket();
    ecos++;
  }

  if (ahora - tTx >= 5){                           // ~200 paquetes por segundo
    tTx = ahora;
    udp.beginPacket(IPAddress(192,168,4,255), UDP_PORT);
    udp.write((const uint8_t*)"CENSO", 5);
    udp.endPacket();
  }

  if (ahora - tInf >= 2000){                       // cuantos pings devolvimos
    tInf = ahora;
    Serial.printf("#STAT,ecos,%lu,estaciones,%d\n", (unsigned long)ecos,
                  WiFi.softAPgetStationNum());
    ecos = 0;
  }

  if (ahora - tLed >= 500){                        // late = está emitiendo
    tLed = ahora;
    led = !led;
    digitalWrite(2, led);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// ROLE 4 — RADAR LD2450: sigue hasta 3 personas con coordenadas reales
//
// El modulo hace todo el trabajo duro y entrega posiciones ya calculadas por
// UART a 256000 baudios, 10 veces por segundo. Esta placa solo traduce.
//
// CABLEADO (el TX va cruzado con el RX; si van derechos no llega NADA):
//
//     LD2450         ESP32 NodeMCU
//     -------        --------------
//     5V     -----> VIN        (5V del USB)
//     GND    -----> GND
//     TX     -----> GPIO16  (RX2)   <-- cruzado
//     RX     <----- GPIO17  (TX2)   <-- cruzado
//
// Trama: AA FF 03 00 + 3 objetivos de 8 bytes + 55 CC = 30 bytes.
// Cada objetivo: X (mm), Y (mm), velocidad (cm/s), resolucion (mm).
// Los enteros vienen en signo-magnitud: el bit mas alto es el SIGNO, no parte
// del numero. Leerlos como complemento a dos da posiciones absurdas.
// ═══════════════════════════════════════════════════════════════════════════
#elif ROLE == 4

#define RADAR_RX   16
#define RADAR_TX   17
#define RADAR_BAUD 256000

static uint8_t tr[64];
static int     largo = 0;
static uint32_t tEstado = 0, tramas = 0;

// signo-magnitud: bit 15 encendido = positivo
static int16_t leerCoord(uint8_t bajo, uint8_t alto){
  int16_t v = ((alto & 0x7F) << 8) | bajo;
  return (alto & 0x80) ? v : -v;
}

void setup(){
  Serial.begin(BAUD);
  delay(300);
  Serial2.begin(RADAR_BAUD, SERIAL_8N1, RADAR_RX, RADAR_TX);
  Serial.println("#ROLE,RADAR");
  Serial.println("#FMT,R,n,x1,y1,v1,x2,y2,v2,x3,y3,v3  (mm y cm/s)");
  Serial.println("#INFO,si no llegan tramas revisa que TX y RX esten CRUZADOS");
}

void loop(){
  while (Serial2.available()){
    uint8_t b = Serial2.read();

    if (largo < 4){                       // buscando la cabecera
      const uint8_t cab[4] = {0xAA, 0xFF, 0x03, 0x00};
      if (b == cab[largo]) tr[largo++] = b;
      else                 largo = (b == 0xAA) ? (tr[0] = 0xAA, 1) : 0;
      continue;
    }

    tr[largo++] = b;
    if (largo < 30) continue;
    largo = 0;
    if (tr[28] != 0x55 || tr[29] != 0xCC) continue;   // cola invalida

    tramas++;
    int activos = 0;
    int16_t x[3], y[3], v[3];
    for (int i = 0; i < 3; i++){
      const uint8_t *o = tr + 4 + i*8;
      x[i] = leerCoord(o[0], o[1]);
      y[i] = leerCoord(o[2], o[3]);
      v[i] = leerCoord(o[4], o[5]);
      if (x[i] || y[i]) activos++;        // ranura vacia = todo en cero
    }
    Serial.printf("R,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                  activos, x[0], y[0], v[0], x[1], y[1], v[1], x[2], y[2], v[2]);
  }

  if (millis() - tEstado >= 2000){
    tEstado = millis();
    Serial.printf("#STAT,tramas,%lu\n", (unsigned long)tramas);
    tramas = 0;
  }
}

#else
  #error "ROLE tiene que ser 1 (SNIFFER), 2 (CSI_RX), 3 (CSI_TX) o 4 (RADAR)"
#endif
