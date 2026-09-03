# Hardware — qué pedir y por qué

> Regla que ordena todo: **el demo actual ya funciona sin hardware.** Todo lo de acá abajo
> es aditivo. Si no llega, no perdés nada. Si llega, se monta como módulo aparte y **no se
> toca el camino que ya anda.**

---

## Prioridad 1 — ESP32 (2 unidades, pedir 3–5)

### Qué te compra

Hoy tu charla dice: *"el WiFi puede hacer esto; acá está el equivalente en el canal acústico"*.
Con ESP32 dice: *"el WiFi hace esto, y lo estás viendo"*.

Es la diferencia entre una analogía y la cosa misma. Es el único upgrade que cambia la
categoría del proyecto, no su calidad.

### Por qué es viable sin backend

**Web Serial API.** Chrome expone `navigator.serial`: el ESP32 escupe CSI por USB en texto, y
la página lo lee directo. Sin Python, sin servidor, sin instalar nada del lado de la laptop.
Entra a la consola que ya existe como una fuente de datos más.

```
ESP32 (sketch CSI) ──USB serial──> navigator.serial ──> misma consola INDEC
```

### Cuántos y cuáles

| Cantidad | Para qué |
|---|---|
| **1** | Mínimo viable. Recibe CSI de las tramas del router de la sala. **Dependés del WiFi del lugar** — que es exactamente lo que no querés en un evento. |
| **2** | **El número correcto.** Uno emite a ritmo fijo, otro recibe. Enlace propio, tasa controlada (100+ paquetes/s), independiente de la sala. |
| **3–5** | Margen por placa fallada, variante equivocada o cable que no aparece. **Es lo que conviene pedir.** |

**Modelos que sirven:** ESP32 clásico (WROOM/WROVER), **ESP32-S3**, ESP32-C3.
Los tres exponen CSI por `esp_wifi_set_csi_rx_cb()`.

**⚠️ ESP8266 NO SIRVE.** No expone CSI. Es el error más frecuente al pedir prestado — si te
ofrecen "unos ESP", confirmá que digan **ESP32**.

**Pedí también los cables USB.** Micro-USB en placas viejas, USB-C en las nuevas. Sin el cable
correcto la placa no hace nada.

### Cronograma realista si llegan

| Paso | Tiempo | Quién |
|---|---|---|
| Arduino IDE + soporte ESP32 (descarga ~200 MB) | 20–40 min | vos, en paralelo |
| Sketch CSI (yo lo escribo) y flasheo | 20 min | vos flasheás |
| Lectura Web Serial + parseo + visual | 60–90 min | yo |
| Calibración y prueba | 30 min | los dos |

**Total ~2,5–3 h con todo saliendo bien.** Sinceramente: si las placas no están en tus manos
con **4 horas de margen antes de presentar**, no lo intentes. El costo de fallar es perder el
ensayo del demo que ya funciona.

### El punto de corte
Si a media tarde no las tenés, **se cancela sin drama** y presentás lo que ya está, que está
completo. El ESP32 es la v2.

---

## Prioridad 2 — Micrófono USB (riesgo cero, ganancia hoy)

El más subestimado de la lista. **Mejora tu demo actual sin tocar una línea de código.**

Los micrófonos de laptop traen procesamiento de fábrica y muchos cortan arriba de 18 kHz —
justo donde vivís. Un USB básico con respuesta plana hasta 20 kHz te sube la relación señal-ruido
inmediatamente, y encima lo podés apuntar hacia donde va a estar el panel opaco.

Cualquiera sirve. No hace falta que sea bueno, hace falta que no filtre.

---

---

## LISTA COMPLETA — qué llevarte

### Lo que hace falta sí o sí (si vas por el ESP32)

| # | Qué | Cantidad | Detalle |
|---|---|---|---|
| 1 | **Placa ESP32** | 2 (pedí 3–5) | ESP32 / S3 / C3. **ESP8266 no.** |
| 2 | **Cable USB de DATOS** | 2 | Del tipo que pida la placa. Ver abajo: el cable es la falla #1. |
| 3 | **Power bank USB** | 1 | Para el emisor. Lo convierte en portátil. Ver "el truco" abajo. |

### El cable: dónde falla todo el mundo

**Hay dos tipos de cable USB y se ven idénticos.** Los de "solo carga" tienen las líneas de datos
sin conectar: enchufás la placa, prende el LED, y la computadora **no la ve**. Perdés media hora
pensando que la placa está rota.

> **Probalo antes de irte:** enchufá la placa y fijate si aparece un puerto COM nuevo en el
> Administrador de dispositivos de Windows. Si no aparece, es el cable (o el driver, ver abajo).

**Cuál te toca según la placa:**

| Placa | Conector |
|---|---|
| ESP32-DevKitC, NodeMCU-32S, WROOM (las clásicas, pre-2022) | **micro-USB** (el de Android viejo) |
| ESP32-S3-DevKitC, ESP32-C3, Arduino Nano ESP32 (las nuevas) | **USB-C** |

Como no sabés cuál te van a prestar, **llevá los dos tipos**. Un micro-USB y un USB-C, ambos
de datos. Son cables que ya tenés en tu casa.

### Driver USB — el otro tropiezo clásico en Windows

La placa habla con la compu a través de un chip conversor USB-serie. En Windows, algunos
necesitan driver:

| Chip | ¿Driver? |
|---|---|
| **CH340 / CH9102** | Sí, casi siempre. Es el más común en placas baratas. |
| **CP2102 / CP2104** | A veces. Driver de Silicon Labs. |
| **USB nativo** (S3 y C3) | **No hace falta.** Otro motivo para preferir S3/C3. |

**Descargalo ahora, antes de tener la placa** — con la placa en la mano y el reloj corriendo es
el peor momento para buscar un driver.

### Flasheo vs. funcionamiento — tus dos preguntas exactas

**"¿Tengo que conectarlas todas juntas a la computadora?"**
**No.** El firmware se pasa **de a una**. Enchufás la primera, flasheás, desenchufás, enchufás la
segunda, flasheás. Con **un solo cable** alcanza para esta parte. Toma ~2 minutos por placa.

**"¿Después necesitan corriente?"**
**Sí, siempre.** El ESP32 no tiene batería. El firmware queda grabado en memoria flash y
sobrevive al apagado, pero **sin 5 V por USB la placa no hace nada**. Cualquier fuente USB
sirve: laptop, cargador de celular, power bank.

**Pero las dos placas no necesitan lo mismo:**

| Placa | Qué necesita | Por qué |
|---|---|---|
| **RECEPTORA** | Cable de **datos** a la laptop, todo el tiempo | Por ahí viaja el CSI hacia el navegador (Web Serial). Sin datos no hay demo. |
| **EMISORA** | **Solo corriente** | Únicamente tira paquetes por aire. No le importa qué la alimenta. |

### 🎯 El truco que te mejora el demo

Como la emisora **solo necesita corriente**, la enchufás a un **power bank** y queda suelta.
Eso te deja ponerla **del otro lado del panel opaco, o del otro lado de la sala**.

Ahora el cuerpo del voluntario pasa **entre las dos placas** — y ahí es donde el sensado por
WiFi da su señal más fuerte. Con las dos placas juntas al lado de la laptop, la señal es mucho
más pobre.

Un power bank de $5 mejora el demo más que una placa extra.

### Resumen de lo que ponés en la mochila

```
2 × ESP32  (pedí 3–5)
1 × cable micro-USB de datos      ← llevá los dos tipos,
1 × cable USB-C de datos          ← no sabés qué placa te dan
1 × power bank
1 × micrófono USB                 ← mejora el demo actual, riesgo cero
    driver CH340 y CP210x ya descargados
```

---

## Kit de escenario — esto no es opcional y no lo piensa nadie

Independiente del ESP32. Si algo de acá falla, **no presentás**.

| Qué | Por qué |
|---|---|
| **Adaptador HDMI para tu laptop** | El que se olvida siempre. Si tu máquina es USB-C, sin adaptador no hay proyector. |
| **Cable HDMI propio** | No confíes en que el lugar tenga uno suelto. |
| **Cargador de la laptop + zapatilla** | El demo come batería. Una hora de espera antes de subir te la funde. |
| **Panel opaco** | Cartón grande, tela oscura, valija abierta. Cualquier cosa que tape. Probalo en casa. |
| **Segundo teléfono para el Velo** | O usás la tecla `V` en la consola. |
| **El video del demo grabado** | En el disco, no en la nube. El WiFi del lugar puede no existir. |
| **La app abierta y calibrada** antes de subir | No calibres con la sala mirando. |

---

## No pedir hoy

### Arduinos — depende de cuál, y hay una excepción importante

| Modelo | ¿Sirve? | Por qué |
|---|---|---|
| Arduino **Uno / Nano / Mega** (AVR) | ❌ No | No tienen radio WiFi. Nada que hacer. |
| Arduino **Nano 33 IoT**, **MKR WiFi 1010** | ❌ En la práctica no | Llevan un módulo u-blox NINA-W102 que **por dentro es un ESP32**, pero el firmware de Arduino no expone CSI. Habría que reflashear el módulo entero. Días. |
| Arduino **Uno WiFi Rev2** | ❌ En la práctica no | Mismo caso (NINA-W13). |
| **Arduino Nano ESP32** | ✅ **SÍ** | Salió en 2023 y **es un ESP32-S3**. Se le carga el mismo firmware. **Si te ofrecen este, aceptalo.** Usa USB-C. |

Traducción para el mensaje: si Pablo dice *"tengo Arduinos"*, preguntá **cuáles**. Solo el
**Nano ESP32** sirve.

### Raspberry Pi

| Modelo | ¿Sirve? | Por qué |
|---|---|---|
| Pi 3B+ / Pi 4 | ⚠️ Técnicamente sí, hoy no | Nexmon CSI funciona sobre el chip bcm43455c0, pero exige firmware parcheado, compilar nexmon y cargar módulos de kernel. **Horas, y falla seguido.** |
| Pi 5 | ❌ | Chip distinto, sin soporte de Nexmon maduro. |
| Pi Pico W | ❌ | El CYW43439 no da acceso a CSI. |

**Para hoy: no.** Vale la pena saber que **para una v2 el Pi es mejor que el ESP32** — Nexmon
da CSI de 80 MHz de ancho de banda contra los 20 MHz del ESP32, con muchas más subportadoras.
Es el camino si esto sigue después del evento.

### Otras plataformas con CSI real (contexto, no para hoy)

| Equipo | Situación |
|---|---|
| **Intel 5300 NIC** | La plataforma clásica de investigación. Notebooks viejas. Requiere Linux y driver parcheado. |
| **Atheros AR9xxx** | Atheros CSI Tool. Mismo requisito: Linux + driver parcheado. |
| **Router con OpenWrt** | Extraer CSI de un router común requiere parchear el driver. Días, no horas. |
| **RTL-SDR** | No da CSI. Mostraría el espectro RF de la sala — lindo, pero es textura, no argumento. |

**Por eso el pedido es ESP32 y no otra cosa:** es el único que va de cero a CSI en un par de
horas sin tocar drivers ni kernel.

---

## Los mails

### Opción A — mail completo

**Asunto:** Consulta: ¿me prestás 2–3 ESP32 por hoy? (proyecto de sensado por WiFi)

> Hola Pablo, ¿cómo estás?
>
> Te escribo medio en frío: no cursé con vos, pero varios amigos sí y siempre hablan muy bien
> de tus materias. Soy Uriel, estudiante de la facultad.
>
> **El pedido concreto:** ¿tendrías 2 o 3 ESP32 para prestarme hoy? Los devuelvo hoy mismo,
> o mañana temprano si preferís.
>
> **Para qué:** estoy armando un detector de presencia humana que no usa cámara. La idea es
> medir cómo un cuerpo deforma el canal físico entre dos dispositivos. Ya lo tengo funcionando
> sobre el canal acústico (un tono de 19 kHz y corrimiento Doppler), pero lo que quiero mostrar
> de verdad es el canal WiFi: leer **CSI** con `esp_wifi_set_csi_rx_cb()` y estimar presencia a
> partir de la variación entre subportadoras. Con dos placas —una emitiendo a tasa fija y otra
> recibiendo— tengo un enlace controlado y no dependo del WiFi del lugar.
>
> Va en esta línea: el sensado por WiFi ya es norma IEEE (802.11bf, ratificada en 2025), y hay
> trabajo publicado de Carnegie Mellon reconstruyendo pose corporal solo con CSI
> (*DensePose From WiFi*). Me interesa llegar hasta donde se pueda con hardware de mostrador.
>
> **El detalle de tiempo:** presento el proyecto **hoy a la tarde**, así que si llegás a
> responderme en las próximas horas me salvás. Si no llegás, no hay problema: la versión
> acústica ya funciona y se presenta igual.
>
> Sirve ESP32, ESP32-S3 o ESP32-C3 (ESP8266 no, ese no expone CSI). Si tenés los cables USB
> a mano, mejor. Paso a buscarlos por donde me digas y te dejo mi documento si querés.
>
> Gracias por leer,
> Uriel

### Opción B — WhatsApp (los profesores contestan más rápido acá)

> Hola Pablo! Soy Uriel, de la facu — no cursé con vos pero varios amigos sí.
>
> Consulta corta: ¿tenés 2 o 3 **ESP32** que me puedas prestar por hoy? Los devuelvo hoy mismo.
>
> Estoy armando un detector de presencia sin cámara, leyendo **CSI de WiFi** para estimar
> ocupación por la variación del canal. Lo tengo andando en el canal acústico y quiero mostrar
> la versión en WiFi, que es a donde apunta el 802.11bf.
>
> **Presento hoy a la tarde**, así que cualquier cosa que puedas es un montón. Si no llegás,
> no hay drama, tengo plan B funcionando.
>
> (ESP32 / S3 / C3 sirven — ESP8266 no expone CSI. Si tenés los cables USB, mejor.)
>
> Paso a buscarlos por donde te quede cómodo. ¡Gracias!

---

### Qué cambié respecto de tu borrador y por qué

| Cambio | Motivo |
|---|---|
| Sacar "sin cámara ni micrófono" | Tu demo actual **sí usa micrófono**. Si te lo marca, perdés credibilidad de entrada. Ahora dice "sin cámara", que es verdad, y explica el canal acústico como lo que es. |
| Nombrar `CSI`, `esp_wifi_set_csi_rx_cb()`, 802.11bf, DensePose | A un profesor de robótica esto le cambia el mail de "un chico quiere cosas" a "este sabe lo que pide". Es lo que más sube la probabilidad de un sí. |
| Bajar de 5 a "2 o 3" | Pedir 5 sin justificar suena a acopio. Dos es el número técnico real. Si te ofrece más, aceptás. |
| Aclarar que ESP8266 no sirve | Te ahorra el viaje al pedo. Es el error más común. |
| Poner el pedido arriba de todo | Un mail que arranca con contexto y llega al pedido en el párrafo cuatro se lee en diagonal. |
| Agregar "si no llegás, no hay problema" | Le saca la presión. Nadie quiere ser responsable de que a alguien le salga mal la presentación — y esa presión hace que no te contesten. |
| Devolución y retiro explícitos | Un préstamo con fecha de devolución clara es infinitamente más fácil de conceder. |
| Fecha y hora concretas | "Hoy a la tarde" da urgencia real sin exigir. |
