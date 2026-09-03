# CENSO FANTASMA

### Dirección Nacional de Teledetección Habitacional — INDEC

**App:** https://justuris.github.io/censo-fantasma/
**Repo:** https://github.com/JustUrIs/censo-fantasma

---

## Qué es

Un instrumento de sonar ultrasónico que corre entero en el navegador, sin instalar nada, y
detecta presencia y movimiento humano midiendo el corrimiento Doppler de su propio eco.

Es el proyecto presentado en la anti-hackathon **SideQuest — Conspiracy Edition**, cuya consigna
es construir la tecnología que haría falta en un mundo donde las teorías conspirativas fueran
reales.

**La premisa:** el formulario del Censo Nacional 2022 nunca fue el censo. Era el **set de
entrenamiento**. La población real se releva por variación del canal físico: toda onda que
atraviesa un ambiente vuelve deformada por los cuerpos que hay adentro. No hace falta cámara,
ni permiso de ubicación, ni que instales nada.

**Por qué la premisa incomoda:** porque la tecnología es real.

| Hecho | Fuente |
|---|---|
| El sensado de presencia por WiFi es **norma IEEE ratificada**. 802.11bf-2025, aprobada el 28/05/2025, publicada el 26/09/2025. Tu próximo router va a ser un radar por especificación | IEEE 802.11bf-2025 |
| Ya está desplegado en **millones de routers y lamparitas inteligentes**, con 92,61% de precisión detectando movimiento humano en hogares reales | arXiv 2506.04322 (jun 2025) |
| Se puede **reconstruir la pose corporal completa** solo con WiFi: a través de paredes, en oscuridad total, varias personas a la vez | DensePose From WiFi, CMU — arXiv 2301.00250 |
| Existe una **contramedida publicada**: una metasuperficie que randomiza el canal y baja el ataque a menos del 5% | IRShield, IEEE S&P 2022 — arXiv 2112.01967 |
| El **rastreo ultrasónico entre dispositivos** ya ocurrió y fue sancionado | SilverPush / FTC 2016; *"Talking Behind Your Back"*, CCS 2017 |

Este proyecto no usa WiFi: usa **sonido**, porque el emisor y el receptor ya están adentro de
cualquier dispositivo. La física es la misma —variación del canal— y la técnica está publicada
(**SoundWave**, Microsoft Research CHI 2012; **ApneaApp**, UW MobiSys 2015).

---

## Qué es medición y qué es puesta en escena

**Esto no se negocia y está declarado en todos los documentos.**

| Real, medido en vivo | Puesta en escena |
|---|---|
| Emisión de 19 kHz, captura, FFT | **El número exacto de habitantes** |
| Corrimiento Doppler por movimiento humano | **El "4" con tres personas** — lo dispara la tecla `G` |
| Dirección del movimiento (se acerca / se aleja) | El modo "relevamiento archivado" (espectro sintético de contingencia) |
| Latch de presencia de 4,2 s | |
| Espectrograma | |
| Velo (jammer de banda ancha) | |
| Detector de balizas ultrasónicas | |

La señal es real. El conteo es la calibración del organismo. Detalle completo en
[`DOC_TECNICA.md`](DOC_TECNICA.md) §11.

---

## Los cuatro módulos

| # | Módulo | Qué hace | Emite | Micrófono |
|---|---|---|---|---|
| **01** | **Consola de censo** | El instrumento completo: espectrograma en vivo, energía Doppler, dirección, contador de habitantes, teclas de operador | tono ~19 kHz | sí |
| **02** | **Terminal ciudadano** | El mismo motor con UI mínima, pensado para el teléfono del público. Movés la mano y el número sube | tono ~19 kHz | sí |
| **03** | **Velo** | Contramedida. Ruido de banda ancha en 17–22 kHz que ahoga cualquier relevamiento acústico del ambiente, incluido el propio | ruido 17–22 kHz | **no** |
| **04** | **Contravigilancia** | Escucha pasiva. Detecta balizas ultrasónicas de terceros operando en el ambiente | nada | sí |

El Velo **no pide micrófono**: el segundo teléfono se activa sin otorgar ningún permiso.

**Enlaces profundos:** `#consola`, `#terminal`, `#velo`, `#contra`. Para el QR del público
conviene apuntar a **`https://justuris.github.io/censo-fantasma/#terminal`**, que cae directo en
la pantalla de autorización del Terminal ciudadano.

---

## Cómo correrlo local

El micrófono **exige contexto seguro** (HTTPS o `localhost`). Abrir el archivo con doble clic
(`file://`) **no funciona**: el navegador ni siquiera ofrece el permiso.

```bash
cd Anti-Hackathon-2
python -m http.server 8000
```

Después abrí:

```
http://localhost:8000/censo_fantasma.html
```

En el repo publicado el archivo se llama `index.html`, así que ahí la URL es la raíz del sitio.
`deploy.sh` hace esa copia y el push.

**Sin dependencias.** No hay `npm install`, no hay build, no hay backend. Un archivo HTML de
826 líneas y 36 KB. La única request externa es la tipografía de Google Fonts; después de cargar
la página, nada sale del dispositivo.

---

## Teclas de operador (solo en la Consola)

| Tecla | Qué hace |
|---|---|
| `1`–`9` | Fija el conteo en ese número |
| `0` | Vuelve a automático |
| `G` | Suma un **habitante no declarado** al conteo automático |
| `C` | Recalibra el piso de ruido |
| `V` | Activa / desactiva el Velo |
| `H` | Muestra u oculta el panel de teclas (se desvanece solo a los 9 s) |

`G` suma **sobre el automático** y no funciona en modo manual. Si con tres personas la consola
marca 2, `G` te da 3, no 4 — para el 4 garantizado, apretá `4`.

---

## Antes de presentar

1. Abrí la app **en tu teléfono**, entrá a `Terminal ciudadano`, aceptá el micrófono y movete la
   mano. El número tiene que subir.
2. Si no sube: bajá la portadora a **17,50 kHz** y subí la potencia. Es el caso más común — los
   micrófonos de teléfono suelen caer arriba de 18 kHz.
3. **Calibrá en la sala real** (tecla `C`) con la gente ya adentro. No es opcional: cien personas
   moviéndose son señal Doppler legítima permanente.
4. **Grabá el demo funcionando en video** antes de salir.
5. Apretá `H` para ocultar el panel de teclas antes de que suba nadie al escenario.

Protocolo de falla completo en [`GUION.md`](GUION.md). Troubleshooting técnico en
[`DOC_TECNICA.md`](DOC_TECNICA.md) §13.

---

## Documentos

| Archivo | Qué contiene | Para quién |
|---|---|---|
| [`DOC_TECNICA.md`](DOC_TECNICA.md) | Cadena de señal completa, fórmulas con sus números, tabla de parámetros, limitaciones honestas, troubleshooting, bugs conocidos | Quien quiera auditar o reconstruir el instrumento |
| [`DOC_NEGOCIO.md`](DOC_NEGOCIO.md) | El organismo como si existiera: objeto, marco normativo sobre la Ley 17.622, cadena de custodia del dato, el Derecho a la Ausencia, y la línea de negocio de la contramedida | Quien quiera el proyecto como institución, no como demo |
| [`DOC_WOW.md`](DOC_WOW.md) | Análisis de impacto separado en dos audiencias —humanos y técnicos— con el mecanismo de cada momento, y las objeciones probables con la respuesta exacta | Quien vaya a presentarlo o a defenderlo |
| [`GUION.md`](GUION.md) | El pitch de escenario, 5 minutos, palabra por palabra, con el protocolo de falla | El presentador |
| [`PLAN_V2.md`](PLAN_V2.md) | Estado actual y hoja de ruta | Contexto de desarrollo |
| [`PLAN_EJECUCION.md`](PLAN_EJECUCION.md) | La decisión técnica: por qué sonar y no WiFi | Contexto de desarrollo |
| [`PLAN_IDEAS.md`](PLAN_IDEAS.md) | Banco de ideas del evento y la fórmula: premisa conspirativa falsa + tecnología SOTA real | Contexto de desarrollo |

---

## Estado

| Componente | Estado |
|---|---|
| Consola, Terminal, Velo, Contravigilancia | Operativos |
| Perfilado de rango por barrido (FMCW) | En desarrollo — requiere salir del `AnalyserNode` a un `AudioWorklet` |
| Rechazo de modo común contra transitorios de banda ancha | Diagnosticado y especificado, **no implementado** |
| Certificado de No Existencia | No implementado |

**Falla conocida:** un golpe en la mesa es banda ancha e inyecta energía en las bandas laterales
que el detector lee como Doppler, así que genera falsos positivos. El fix correcto es rechazo de
modo común contra una banda de referencia lejana, no un umbral más alto.
[`DOC_TECNICA.md`](DOC_TECNICA.md) §9.2.

---

## Referencias

- **IEEE 802.11bf-2025** — WLAN Sensing. Aprobado 28/05/2025, publicado 26/09/2025.
- **arXiv 2506.04322** (jun 2025) — WiFi sensing desplegado a escala; 92,61% de precisión.
- **DensePose From WiFi** — Carnegie Mellon, **arXiv 2301.00250**.
- **IRShield** — IEEE Security & Privacy 2022, **arXiv 2112.01967**.
- **SilverPush / FTC 2016**; *"Talking Behind Your Back"*, CCS 2017.
- **SoundWave** — Microsoft Research, CHI 2012.
- **ApneaApp** — University of Washington, MobiSys 2015.
- **Ley 17.622** (1968) — creación del INDEC; art. 10, secreto estadístico.

---

> La Dirección Nacional de Teledetección Habitacional no existe. La Ley 17.622 sí.
> Los cinco papers también.
