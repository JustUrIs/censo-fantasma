# Cómo armar el sistema WiFi — paso a paso

**Tu inventario:**
- ESP32 **emisora** + cable
- ESP32 **receptora** + cable
- ESP32 receptora extra, **sin cable**
- Arduino Mega + shield WiFi, sin firmware
- 2 computadoras

---

## Lo primero: dejá el Arduino a un lado

**El Arduino Mega con shield WiFi no sirve para esto.** No es cuestión de firmware.

El truco de todo el sistema es leer el **CSI** — la medición fina de cómo llega la señal WiFi.
Ese dato solo lo entrega el chip del ESP32. El shield del Mega usa otro chip (casi seguro un
ESP8266 o el viejo shield oficial), y **ninguno de los dos lo expone.**

Tampoco necesitás "un Arduino normal". **Con los dos ESP32 alcanza.**

---

## Lo segundo: no necesitás dos computadoras

Esta es la regla que te ordena todo:

| Placa | Qué necesita | Dónde se enchufa |
|---|---|---|
| **Emisora** | **Solo corriente** | Cargador de pared, power bank, cualquier USB |
| **Receptora** | **Corriente + datos** | La computadora que tiene el navegador abierto |

La emisora no le manda nada a ninguna computadora. Solo grita al aire. **Enchufala a la pared.**

### ¿A qué computadora va la receptora?

**A esta — la que tiene el driver instalado.**

El driver del chip USB (CP2102) lo instalé en esta máquina. En la otra computadora, si no lo
tiene, la receptora ni aparece: Windows la ve como "dispositivo desconocido". Es exactamente lo
que te pasó el primer día.

### ¿Se pueden enchufar varias en la misma compu?

**Sí.** Cada una aparece como un puerto COM distinto. Pero hoy la app lee **una placa por vez**,
así que por ahora: una receptora en la compu, la emisora en la pared.

La tercera ESP32 (la que no tiene cable) todavía no hace falta. Sirve para el mapa de zonas,
que viene después.

---

## El montaje

```
   [EMISORA]                              [RECEPTORA]
   enchufada a la pared                   cable de datos
   a ~1 m del piso                        a esta laptop
        \                                      /
         \______ 2 a 3 metros de aire _______/
                         |
              acá camina / se sienta la persona
```

**La persona tiene que estar ENTRE las dos.** Ahí la señal se deforma más.
Si las ponés juntas al lado de la compu, casi no vas a ver nada.

- Las dos a la **misma altura**, más o menos a la del pecho de alguien sentado
- Nada metálico grande en el medio (heladera, estantería de metal)
- **Para atravesar una pared:** una de cada lado

---

## Encenderlo — en este orden

**1 · Emisora a la pared.**
El LED azul tiene que **parpadear cada medio segundo**. Eso significa que está emitiendo.
Si no parpadea, no tiene el firmware de emisora.

**2 · Esperá 5 segundos.**
Para que levante su red (se llama `CENSO_FANTASMA`).

**3 · Receptora a esta laptop**, con el cable de datos.

**4 · Confirmá que la ve:**
```powershell
cd "C:\Users\justu\Downloads\Anti-Hackathon-2\firmware"
.\que_es.ps1
```
Tiene que decir **"ESTA PLACA ES: RECEPTORA CSI"** y mostrar líneas que empiezan con `C,`.

> **Si arrancó antes que la emisora:** con el firmware nuevo sigue buscando sola. Con el
> firmware viejo, apretá el botón **EN** (o **RST**) de la receptora y listo.

**5 · Abrí la app:**
`https://justuris.github.io/censo-fantasma/#csi` → **Conectar placa** → elegí el COM.

**6 · Qué tenés que ver:**
- Arriba dice **"Perturbación del canal"**
- Una cascada de 64 líneas moviéndose
- Abajo: **Tramas por segundo** en 40 o más

Quedate quieto 10 segundos (aprende cómo es el canal vacío). Después caminá entre las dos
placas: **la cascada se tiene que retorcer y el número subir.**

---

## Actualizar la receptora (recomendado)

Le arreglé un bug: si arrancaba antes que la emisora, se rendía y había que apretar RESET.
Para cargarle el arreglo, **con la receptora enchufada a esta laptop**:

```powershell
cd "C:\Users\justu\Downloads\Anti-Hackathon-2\firmware"
.\flash.ps1 2
```

Un minuto. Solo la receptora — la emisora no cambió.

---

## Si algo no anda

| Pasa esto | Es esto | Hacé esto |
|---|---|---|
| `que_es.ps1` dice "no hay ninguna placa" | Cable de solo carga, o compu sin driver | Otro cable, o enchufala a **esta** laptop |
| Dice "no pude identificarla" | La emisora no está, o está lejos | Mirá que el LED de la emisora parpadee |
| La app conecta pero "tramas por segundo" en 0 | La receptora no encontró a la emisora | Acercalas; con firmware viejo, apretá EN |
| La cascada está quieta aunque camines | No estás pasando entre las dos | Parate en la línea que las une |
| "El puerto está ocupado" | La app lo tiene tomado | En la app, **Desconectar**; o cerrá la pestaña |

Cuando tengas la cascada moviéndose, grabá un video corto y avisame: ese es el primer momento
en que el sistema WiFi funciona de punta a punta.
