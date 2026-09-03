# CENSO FANTASMA — Documentación técnica

**Qué es esto.** La descripción exacta de cómo funciona el instrumento: qué mide, con qué
fórmulas, con qué constantes y hasta dónde llega. Está escrito para alguien que nunca vio el
proyecto y quiere auditar el código o reconstruirlo.

**Fuente de verdad:** `censo_fantasma.html` (un archivo, 826 líneas, sin dependencias).
**App publicada:** https://justuris.github.io/censo-fantasma/

**Advertencia de honestidad, arriba de todo.** La señal es real y la podés verificar. El
*número exacto de habitantes* no sale de la señal: es puesta en escena, la dispara el
operador con el teclado. En la sección [11](#11-lo-que-es-medición-y-lo-que-es-teatro) está
detallado línea por línea qué es qué.

---

## 1. La física, en una página

El instrumento emite una portadora `f₀` de ~19 kHz por el parlante y escucha con el micrófono
del mismo dispositivo. Un reflector que se mueve corre en frecuencia el eco que devuelve:

```
Δf = 2 · v · f₀ / c        c = 343 m/s (velocidad del sonido en aire)
```

Con `f₀ = 19.000 Hz` y `c = 343 m/s`:

```
Δf / v = 2 · 19000 / 343 = 110,8 Hz por cada m/s
```

| Movimiento | v | Δf |
|---|---|---|
| Mano lenta delante de la pantalla | 0,13 m/s | 14,6 Hz |
| Mano a velocidad normal | 0,5 m/s | 55,4 Hz |
| Persona caminando | 1,2 m/s | 133 Hz |
| Techo de lo que el instrumento mira | 1,85 m/s | 205 Hz |

El factor 2 es porque el camino es de ida y vuelta: el cuerpo funciona como reflector móvil,
así que el corrimiento se aplica dos veces.

El instrumento **no mide `Δf`**. Mide la **energía total** que aparece en las bandas laterales
alrededor de la portadora. Es una decisión deliberada: la energía lateral es robusta y barata,
el pico de `Δf` es frágil porque un cuerpo real produce un continuo de velocidades (torso,
brazos, piernas, todos a velocidades distintas al mismo tiempo).

La técnica es la de **SoundWave** (Microsoft Research, CHI 2012) y **ApneaApp** (University of
Washington, MobiSys 2015). No es una invención de este proyecto.

---

## 2. Arquitectura

Un archivo HTML. Sin build, sin `npm`, sin framework, sin backend, sin librerías, sin red
después de cargar la página. Todo el procesamiento ocurre en el dispositivo.

```
censo_fantasma.html
├── <style>            estética de repartición pública (IBM Plex Mono / Sans Condensed)
├── <div id="app">     6 vistas:  land · consola · radar · solo · cert · gate
└── <script>           motor Doppler + radar FMCW + espectrograma + certificado
```

**Seis vistas** (`land`, `consola`, `radar`, `solo`, `cert`, `gate`) que se alternan con el
atributo `hidden`. **Seis módulos:**

| Módulo | Vista | Emite | Micrófono | Notas |
|---|---|---|---|---|
| 01 `consola` | `consola` | tono `f₀` | sí | Doppler + conteo |
| 02 `terminal` | `solo` | tono `f₀` | sí | el del público |
| 03 `velo` | `solo` | ruido 17–22 kHz | **no** | contramedida |
| 04 `contra` | `solo` | nada | sí | escucha balizas ajenas |
| 05 `radar` | `radar` | barrido 17→22 kHz | sí | **solo en laptop** (ver sección 10) |
| 06 `cert` | `cert` | nada | **no** | trámite, sin audio |

Que el Velo **no** pida micrófono es importante en escenario: el segundo teléfono no tiene que
otorgar ningún permiso, se activa y listo. El Certificado tampoco pide nada, así que el público
puede sacarlo aunque haya denegado el micrófono.

**Advertencia de empaquetado.** La fuente se mantiene sin `<!doctype>`, `<html>`, `<head>` ni
`<body>` porque el publicador de Artifacts inyecta su propio esqueleto y prohíbe los propios.
GitHub Pages sirve el archivo crudo, así que `deploy.sh` **inyecta el esqueleto al desplegar**.
Sin `<meta name="viewport">` los navegadores móviles asumen ~980 px y el bloque
`@media(max-width:820px)` nunca se activa — justo en el teléfono del público.

Que el Velo **no** pida micrófono es importante en escenario: el segundo teléfono no tiene que
otorgar ningún permiso, se activa y listo.

**Ruteo por hash.** `location.hash` guarda el modo. `#consola`, `#terminal`, `#velo`, `#contra`
y `#radar`/`#cert` son enlaces profundos válidos: al abrirlos se cae directo en la pantalla de autorización de ese
módulo. **El QR del público conviene apuntarlo a `…/#terminal`**, así nadie tiene que buscar el
botón.

---

## 3. Cadena de señal, etapa por etapa

### 3.1 Emisión — `emitOn()`

```js
S.osc = ctx.createOscillator();  S.osc.type = "sine";
S.osc.frequency.value = S.f0;                    // 19.000 Hz por defecto
S.oscGain.gain.value  = S.gain;                  // 0,07 por defecto (rango 0–0,30)
S.osc.connect(S.oscGain).connect(ctx.destination);
```

Senoidal pura. La ganancia arranca baja (0,07) por dos razones: a 19 kHz un parlante barato
distorsiona y la distorsión mete armónicos e intermodulación que ensucian justo las bandas que
vamos a leer; y porque parte del público **sí escucha 19 kHz** — la audición de alta frecuencia
se degrada con la edad, pero mucha gente de veintipico lo oye.

El deslizador de **portadora** va de 16,5 a 21 kHz. Existe porque muchos micrófonos de teléfono
tienen su propio filtro anti-alias y caen fuerte arriba de 18–19 kHz. Si el instrumento no ve
nada, bajar a 17,50 kHz suele resolverlo (al costo de que más gente lo escuche).

### 3.2 Captura — `audioUp()` y las tres banderas que hay que apagar

```js
navigator.mediaDevices.getUserMedia({ audio:{
  echoCancellation:false, noiseSuppression:false, autoGainControl:false,
  googEchoCancellation:false, googNoiseSuppression:false, googAutoGainControl:false
}});
```

Las tres por defecto vienen **encendidas** y **cada una destruye la medición por un motivo
distinto**:

| Bandera | Qué hace | Por qué mata la medición |
|---|---|---|
| `echoCancellation` | Filtro adaptativo que resta del micrófono lo que salió por el parlante | Nuestra señal **es** lo que salió por el parlante. El AEC está diseñado exactamente para borrar lo único que queremos medir. |
| `noiseSuppression` | Suprime ruido estacionario | Un tono fijo de 19 kHz que suena sin parar es la definición textual de ruido estacionario. El supresor lo identifica y lo baja. |
| `autoGainControl` | Ajusta la ganancia de entrada en el tiempo | Es el peor de los tres. Variar la ganancia es **modular en amplitud** la portadora, y la modulación de amplitud **genera bandas laterales**. El AGC no atenúa la señal: **fabrica falso Doppler**. |

Las variantes con prefijo `goog` son por Chrome viejo, que las leía con ese nombre. Cuestan
nada y cubren un caso de borde.

### 3.3 FFT

```js
S.analyser.fftSize = FFT;                  // 16384
S.analyser.smoothingTimeConstant = 0;      // sin promediado entre cuadros
S.binHz = ctx.sampleRate / FFT;
```

Con `sampleRate = 48.000 Hz`:

```
Resolución    = 48000 / 16384       = 2,93 Hz/bin
Ventana       = 16384 / 48000       = 341 ms
Bins totales  = FFT/2               = 8192
```

Con 44.100 Hz: 2,69 Hz/bin y 371 ms de ventana. El código lee `ctx.sampleRate` y lo muestra en
la tira inferior; no asume nada.

`smoothingTimeConstant = 0` es obligatorio: el promediado del `AnalyserNode` es un filtro
pasabajos sobre el espectro que aplasta justo los transitorios que definen la presencia.

El `AnalyserNode` aplica una **ventana de Blackman** por especificación del Web Audio API. Eso
importa para la sección siguiente: el lóbulo principal de Blackman ocupa ±3 bins y el primer
lóbulo lateral está 58 dB abajo.

### 3.4 Energía de bandas laterales — `measure()`

```js
const c = Math.round(S.f0 / S.binHz);            // bin de la portadora
const p = i => Math.pow(10, S.freqs[i] / 10);    // dBFS → potencia lineal

let up=0, lo=0, car=0;
for(let k=-2;k<=2;k++)        car += p(c+k);     // portadora
for(let k=GUARD;k<=SPAN;k++){ up  += p(c+k);     // banda superior (se acerca)
                              lo  += p(c-k); }   // banda inferior (se aleja)
const side  = up + lo;
const ratio = side / Math.max(car, 1e-20);
```

`GUARD = 5` y `SPAN = 70`. **No son números elegidos a oído — salen de la física.**

```
GUARD =  5 bins → ±14,6 Hz → v = 0,13 m/s   ← piso
SPAN  = 70 bins → ±205  Hz → v = 1,85 m/s   ← techo
```

- **`GUARD` (piso).** Excluye ±14,6 Hz alrededor de la portadora por **fuga espectral**: aunque
  la fuente sea un tono puro, el enventanado reparte energía en los bins vecinos. Con Blackman
  el lóbulo principal llega a ±3 bins, así que `GUARD = 5` deja el análisis fuera del lóbulo
  con dos bins de margen. Debajo de 0,13 m/s no se puede distinguir un cuerpo de la fuga.
- **`SPAN` (techo).** 1,85 m/s es más rápido de lo que camina una persona en un ambiente. Mirar
  más lejos no agregaría cuerpos, agregaría ruido.

Esa es la banda de interés y está justificada de punta a punta: **±14,6 a ±205 Hz es
exactamente el rango de velocidades que puede producir un cuerpo humano adentro de una casa.**

### 3.5 Normalización contra la portadora

`ratio = side / car` es la parte del diseño que más trabajo hace y menos se ve.

La portadora recibida y las bandas laterales pasan por el **mismo** parlante, el **mismo**
aire y el **mismo** micrófono. Si subís el volumen, si el teléfono está más lejos, si el
micrófono es más sensible, **suben las dos juntas** y el cociente no se mueve. La medición se
autocalibra contra el nivel absoluto y contra la ganancia del hardware.

Es lo que permite que el mismo código corra sin tocar nada en una laptop proyectada y en
ochenta teléfonos distintos.

### 3.6 Piso calibrado — `calibrate()`

```js
if(S.calibrating){
  S.calSamples.push(ratio);
  if(S.calSamples.length >= 16){
    const s = S.calSamples.slice().sort((x,y)=>x-y);
    S.base = s[Math.floor(s.length/2)];          // mediana
    S.calibrating = false;
  }
}
```

Junta 16 cuadros y se queda con la **mediana**, no con el promedio. La mediana ignora los
valores extremos: si alguien tose o se mueve durante la calibración, ese cuadro no arrastra el
piso. Con un promedio, un solo golpe fuerte te deja el piso alto y el instrumento ciego el
resto de la sesión.

La calibración se relanza sola cuando cambiás la portadora (porque cambia el bin de la
portadora y con él la fuga), y a mano con el botón *Recalibrar* o la tecla `C`.

### 3.7 La lectura

```js
db  = 10 * log10(ratio / base);            // cuánto subió respecto del piso del ambiente
dir = (up - lo) / (up + lo);               // asimetría de bandas, en [-1, +1]
```

`db` es la magnitud: **cuánta energía lateral hay ahora comparada con la del ambiente vacío**.

`dir` es la dirección. Si el reflector se acerca, el eco vuelve corrido hacia arriba y la banda
superior pesa más → `dir > 0` → `SE ACERCA`. Si se aleja, al revés. Con `|dir| < 0,08` se
declara `ESTACIONARIO`. **Esto es medición real**, no decoración: es la asimetría física del
efecto Doppler.

### 3.8 Latch y conteo — `tally()`

```js
if(db > S.thr) S.lastHit = now;
S.peak = Math.max(db, S.peak * 0.985);           // pico con decaimiento
const held = (now - S.lastHit) < S.HOLD;         // HOLD = 4200 ms

if(S.forced !== null) return S.forced;           // conteo fijado por el operador
if(!held){ S.peak = Math.min(S.peak, S.thr); return 0; }

const p = S.peak, t = S.thr;
let n = 1;
if(p > t * 2.4) n = 2;
if(p > t * 3.6) n = 3;
if(p > t * 5.0) n = 4;
return n + S.ghost;                              // ghost = habitante no declarado (tecla G)
```

**El latch (`HOLD = 4200 ms`) es real y es como funcionan los sensores de ocupación de verdad.**
Un cuerpo humano no se mueve todo el tiempo: alguien parado quieto respira y se balancea, pero
hay segundos enteros sin señal. Sin latch, el contador parpadearía entre 1 y 0 sin parar. Con
latch, subir es inmediato y bajar tarda 4,2 s desde el último cuadro sobre umbral. En escenario
eso se traduce en: entra alguien → sube ya; sale → baja a los 4 segundos. Los dos momentos se
leen desde la platea.

**El decaimiento del pico** (`× 0,985` por cuadro, a ~60 fps) da una vida media de 0,76 s. Es lo
que hace que el número no salte con cada golpe de aire.

**La escalera de conteo es la parte inventada.** Con el umbral por defecto (3,5 dB) las
fronteras caen en 8,4 / 12,6 / 17,5 dB. Más energía lateral **no significa más personas**:
significa más superficie reflectante moviéndose más rápido. Una persona agitando los dos brazos
produce más energía que tres personas quietas. La escalera es plausible, no es medición.

---

## 4. Espectrograma — `wfPush()`

Cada cuadro se corre el lienzo un píxel a la izquierda (`drawImage(cv, -1, 0)`) y se dibuja una
columna nueva a la derecha. Es una cascada de tiempo real, no un gráfico redibujado.

```
y = 0     → bin c + SPAN   → +205 Hz → SE ACERCA
y = alto  → bin c − SPAN   → −205 Hz → SE ALEJA
```

Mapeo de intensidad:

```js
t = clamp((dBFS + 118) / 52, 0, 1);      // ventana dinámica: −118 a −66 dBFS
if(|k| <= 2) t = min(t, 0.55);           // atenúa la portadora para que no sature
e = t*t;
R = 20 + 232·e ;  G = 24 + 139·e² ;  B = 30 + 50·e³
```

La curva `e = t²` y las potencias distintas por canal son lo que da la rampa negro → rojo →
ámbar. El clamp de la portadora existe porque el bin central está 60–80 dB por encima de todo
lo demás: sin atenuarlo, la línea del medio sería una banda blanca que tapa el resto.

---

## 5. Módulo Velo — `jamOn()`

```js
buffer = 2 s de ruido blanco, en loop
highpass @ 16.800 Hz, Q=1  →  highpass @ 16.800 Hz, Q=1  →  gain 0,30  →  salida
```

Dos biquads de segundo orden en cascada = **pendiente de 24 dB/octava**. Debajo de 16,8 kHz cae
rápido, así que el ruido es prácticamente inaudible y arriba de 17 kHz es un piso plano.

**Por qué funciona de verdad.** El instrumento mide `side / car`. El Velo sube `side` en toda la
banda sin tocar `car`. El cociente se dispara, el piso calibrado deja de tener sentido, y
cualquier lectura posterior es basura. No apaga el sensor: lo **ahoga**. El sensor sigue
midiendo — mide ruido.

Es el análogo acústico de **IRShield** (IEEE S&P 2022, arXiv 2112.01967), que hace lo mismo
sobre el canal WiFi con una metasuperficie que randomiza el multipath.

La ganancia 0,30 es alta a propósito: el Velo tiene que ganarle a la portadora del otro
dispositivo desde varios metros.

## 6. Módulo Contravigilancia

No emite nada. Barre 17–22 kHz buscando **picos angostos** con prominencia local:

```js
around = (freqs[i-4] + freqs[i-3] + freqs[i+3] + freqs[i+4]) / 4;
if(freqs[i] - around > 14 && freqs[i] > -92) → candidato
// candidatos a menos de 180 Hz entre sí se funden en uno solo
```

Dos condiciones simultáneas: **14 dB de prominencia** sobre sus vecinos (un tono es angosto; el
ruido ambiente es ancho) y **piso absoluto de −92 dBFS** (para no reportar el pasto). La fusión
a 180 Hz evita contar un mismo tono cinco veces por sus bins vecinos.

**Esto no es ficción.** El rastreo ultrasónico entre dispositivos (uXDT) existe: el caso
**SilverPush** terminó en acción de la **FTC en 2016**, y está documentado académicamente en
*"Talking Behind Your Back"* (CCS 2017). Apps que escuchaban tonos inaudibles emitidos por
publicidades de TV para cruzar identidad entre dispositivos.

---

## 7. Modo archivo — `synthSpectrum()`

Si el micrófono no abre, la pantalla de autorización ofrece **"Abrir relevamiento archivado"**.

**Qué es en realidad: un espectro sintético.** No hay grabación de ninguna sesión previa. La
función arma un espectro plausible cuadro a cuadro:

```js
piso        = −114 + rand·4  dBFS
portadora   = −38  + rand·2  dBFS   (bins c±2)
laterales   = −114 + 62 · exp(−k/26) · nivel · sesgo    (k = 3 … 82)
```

El `nivel` lo conduce el operador con las teclas `1`–`9` (curva `[0, .34, .55, .70, .85, .92,
.95, .97, .99, 1]`, con suavizado exponencial al 7% por cuadro). El `sesgo` oscila lento y hace
que la dirección derive sola entre *se acerca* y *se aleja*, para que no se vea mecánico.

**Es un plan de contingencia teatral, no un instrumento.** La tira inferior lo dice:
`REPRODUCIENDO ARCHIVO`. En la ficción del organismo eso es normal — una repartición mostrando
datos censales archivados. Fuera de la ficción, sépalo quien lea este documento: en modo
archivo **no se está midiendo nada**.

---

## 8. Tabla de parámetros

### Constantes del código

| Constante | Valor | Qué hace | Qué pasa si la tocás |
|---|---|---|---|
| `C_SOUND` | 343 m/s | Velocidad del sonido, para las conversiones Δf↔v | No cambia la medición, solo las etiquetas |
| `FFT` | 16384 | Tamaño de la FFT | Más chico: peor resolución, ventana más corta, más reactivo. Más grande: mejor resolución, más latencia |
| `GUARD` | 5 bins | Bins excluidos a cada lado de la portadora | Menos: entra fuga espectral, falsos positivos con el ambiente quieto. Más: se pierde el movimiento lento |
| `SPAN` | 70 bins | Bins analizados a cada lado | Más: entra ruido de banda ancha. Menos: se pierde la gente caminando rápido |
| `S.HOLD` | 4200 ms | Latch de presencia | Menos: el contador parpadea. Más: tarda demasiado en volver a 0 y el momento "y sabemos cuándo se fue" se pierde |
| decaimiento del pico | ×0,985/cuadro | Vida media 0,76 s a 60 fps | Más cerca de 1: el número se queda pegado arriba |
| escalera de conteo | ×2,4 / ×3,6 / ×5,0 del umbral | Fronteras 1→2→3→4 | Es la parte dramatúrgica; movela a gusto |
| calibración | 16 muestras, mediana | Fija el piso de ruido | Menos muestras: piso inestable |
| `smoothingTimeConstant` | 0 | Sin promediado entre cuadros | >0 aplasta los transitorios |

### Controles de pantalla (Consola)

| Control | Rango | Por defecto | Cuándo tocarlo |
|---|---|---|---|
| Portadora | 16,50 – 21,00 kHz (paso 0,25) | 19,00 kHz | Bajala si el micrófono no ve la portadora. Recalibra sola al moverla |
| Potencia de emisión | 0 – 0,30 | 0,07 | Subila si el nivel no despega; ojo con la distorsión del parlante |
| Umbral de detección | 1 – 14 dB (paso 0,5) | 3,5 dB | Subilo si la sala está ruidosa y detecta sin parar |

### Teclas de operador (solo en la Consola)

| Tecla | Qué hace |
|---|---|
| `1`–`9` | Fija el conteo en ese número (modo manual) |
| `0` | Vuelve a automático |
| `G` | Alterna el **habitante no declarado**: +1 sobre el conteo automático |
| `C` | Recalibra el piso de ruido |
| `V` | Activa / desactiva el Velo |
| `H` | Muestra u oculta el panel de teclas (se desvanece solo a los 9 s) |

**Detalle operativo que hay que conocer.** `G` suma **sobre el conteo automático**, y no hace
nada en manual (si `forced !== null`, `tally()` devuelve el número fijado antes de sumar el
fantasma). Además, si no hay detección sostenida, `G` tampoco suma: el fantasma solo aparece
encima de un cuerpo realmente detectado. Consecuencia práctica: si con tres personas atrás del
panel la consola marca 2, `G` te da **3, no 4**. Si querés el 4 garantizado, apretá `4`.

---

## 9. Limitaciones honestas

Esta sección existe para que nadie te agarre en falta.

1. **El Doppler no ve un cuerpo inmóvil.** Es la limitación estructural del método, no un bug.
   Sin velocidad radial no hay corrimiento y no hay nada que medir. Una persona sentada muy
   quieta desaparece a los 4,2 s del latch. La salida a esto existe y es FMCW + detección de
   respiración (sección 10), y no está implementada.

2. **Los transitorios de banda ancha — resuelto por rechazo de modo común.** Un golpe en la
   mesa, una silla que se arrastra, una tos: son **banda ancha**, inyectan energía en todo el
   espectro, incluida la banda lateral que leemos como Doppler. Era el problema conocido más
   grande del build, y **está corregido en el código publicado**.

   El fix no es un umbral más alto. Es un canal de referencia:

   ```
   Un cuerpo humano no puede poner energía en ±800…±1500 Hz:
   v = Δf·c / (2·f₀) = 800·343 / (2·19000) = 7,22 m/s = 26 km/h.
   Un golpe sí pone energía ahí, porque es banda ancha.

   si (la banda de referencia sube junto con las laterales) → transitorio → descartar
   si (solo suben las laterales)                            → Doppler     → contar
   ```

   Implementación (`REF_A=273`, `REF_B=512` en bins de 2,93 Hz):

   ```js
   dbSide = 10·log10(ratio  / base)      // bandas laterales Doppler
   dbRef  = 10·log10(rRef   / baseRef)   // referencia, fuera del rango humano
   db     = dbSide − dbRef               // rechazo de modo común
   ```

   Ambos pisos se calibran juntos por mediana de 16 muestras. La consola expone `dbRef` y
   una fila `Rechazo modo común` que marca `TRANSITORIO DESCARTADO` cuando la referencia
   supera 2,5 dB, así que el descarte es auditable en vivo.

   **Refuerzo adicional — ocupación sostenida.** El conteo dejó de salir de un pico
   instantáneo. Ahora una media móvil exponencial (α=0,06) alimenta un nivel que sube tras
   ~20 cuadros sostenidos y suelta tras ~110. Es la histéresis de un sensor de ocupación
   real, y es lo que impide que un solo evento salte a 4.

   **Lo que queda:** un golpe con fuerte inclinación espectral hacia agudos puede sobrevivir
   parcialmente al rechazo. La referencia lo atenúa, no lo anula.

3. **No hay reconstrucción 2D.** El Doppler te dice *que algo se mueve* y *hacia dónde* respecto
   del eje parlante–micrófono. **No te dice dónde está.** Un plano de la habitación necesita un
   arreglo de micrófonos (para tener dirección de arribo); el cuerpo calcado necesita CSI de
   WiFi más una red entrenada, que es lo que hace *DensePose From WiFi*. Ninguna de las dos cosa
   está acá.

4. **El conteo exacto no es medición.** Ver sección 3.8 y sección 11.

5. **Cuadros muy correlacionados.** La ventana es de 341 ms y el bucle corre a ~60 fps: dos
   cuadros consecutivos comparten el 98% de las muestras. El instrumento **parece** entregar 60
   mediciones por segundo, pero mediciones independientes hay ~3 por segundo. Todo lo que se ve
   más rápido que eso es interpolación de la ventana deslizante.

6. **Sin fase en el canal Doppler.** `getFloatFrequencyData()` devuelve solo magnitud, así
   que por esa vía no hay procesamiento coherente. **El radar FMCW no usa esa vía**: sale a PCM
   crudo por `ScriptProcessorNode` y aplica filtro adaptado en cuadratura, con fase (sección 10).

7. **A través de paredes: no.** El sonido a 19 kHz no atraviesa una pared de material. Si te
   preguntan esto, la respuesta honesta es: *el sonido no; el WiFi sí, y ese es el punto de
   802.11bf y del paper de CMU*.

8. **Sala llena = ambiente hostil.** Cien personas moviéndose es señal Doppler legítima
   permanente. Calibrar en la sala con la gente ya adentro (`C`) no es opcional.

---

## 10. Radar FMCW de rango (implementado)

En vez de un tono fijo, emitir un **barrido lineal 17 → 22 kHz** que se repite, y correlacionar
lo que vuelve contra la plantilla del barrido. Eso ya no es "hay movimiento": es **distancia**.

```
Ancho de banda        B  = 5.000 Hz
Resolución de rango   = c / (2·B)   = 343 / 10000  = 3,4 cm
Paso por muestra      = c / (2·fs)  = 343 / 96000  = 3,57 mm
Alcance útil          ≈ 2,8 m  (800 retardos)
Período de barrido    = 1024 muestras = 21,3 ms → 47 barridos/s
```

Cómo se vuelve una escena:

1. Correlación → **perfil de rango** (energía reflejada vs distancia).
2. El pico más fuerte es el **camino directo** parlante→micrófono. Ese es el cero de la regla.
3. Se promedia el perfil en el tiempo → eso es el **clutter estático**: paredes, mesa, monitor.
4. **Se resta.** Lo que queda es lo que se mueve.
5. Se apila en el tiempo → **cascada rango-tiempo**: la traza del cuerpo acercándose.

Eso es un radar. Es 1D (distancia), no 2D (planta). Decirlo en escenario suma, no resta.

### Cómo quedó resuelto

**Fase.** No se usa `AnalyserNode` — se sale a PCM crudo con un `ScriptProcessorNode` de 4096
muestras y se aplica **filtro adaptado en cuadratura**: la plantilla se genera en seno y coseno,
y la magnitud `√(I²+Q²)` es la envolvente del eco. Procesamiento coherente, con fase.

**Continuidad de fase del barrido.** Un barrido que se repite con salto de fase produce un
chasquido a 47 Hz, audible. Se evita eligiendo `F2` tal que el barrido cierre en un número
entero de ciclos:

```js
cyc = round((N/fs)·(F1+F2)/2)      // ciclos por barrido, entero
F2  = 2·cyc·fs/N − F1              // F2 corregido
```

A 48 kHz con N=1024 y F1=17 kHz da exactamente 416 ciclos, así que el bucle es silencioso.

**Costo.** El bloque de captura (4096) es múltiplo exacto del barrido (1024), de modo que el
desfase entre bloque y barrido **no cambia entre cuadros**: el camino directo cae siempre en el
mismo retardo. Se busca en todo el rango solo los primeros 6 cuadros y después se sigue una
ventana de ±16. Baja de **3,2M a ~1,2M multiplicaciones por cuadro**, ~14 cuadros/s.

`ScriptProcessorNode` está deprecado y corre en el hilo principal — de ahí que el radar viva
**solo en la Consola** y no en el Terminal ciudadano. Migrar a `AudioWorklet` es la mejora
siguiente y saca el cómputo del hilo de UI.

**Clutter.** El promedio corre con α=0,12 los primeros 90 cuadros (aprende rápido) y con
α=0,012 después (se aquieta). Por eso hay que quedarse quieto ~3 s al arrancar, y por eso
existe la tecla `K` para reaprender si se movió el mobiliario o la laptop.

### Lo que sigue abierto

Detección de **frecuencia respiratoria** (~0,2–0,3 Hz) de un cuerpo quieto: es lo que hace
ApneaApp y es lo que rompería la limitación 9.1 — el Doppler no ve a alguien inmóvil, pero el
FMCW sí puede ver su tórax. Necesita seguir el bin de rango del blanco a lo largo de decenas de
segundos y hacerle un análisis espectral de baja frecuencia. No está implementado.

---

## 11. Lo que es medición y lo que es teatro

| Elemento | Estado |
|---|---|
| Emisión de 19 kHz + captura + FFT | **Real** |
| Corrimiento Doppler por movimiento humano | **Real, medido en vivo** |
| Dirección del movimiento (acerca / aleja) | **Real** — asimetría de bandas |
| Latch de presencia de 4,2 s | **Real** — así funcionan los sensores de ocupación |
| Espectrograma | **Real** — es el espectro, sin retoque |
| Velo (jammer) | **Real** — sube `side` sin tocar `car` |
| Detector de balizas ultrasónicas | **Real** |
| 802.11bf, DensePose From WiFi, IRShield, SilverPush | **Reales y verificables** |
| **El número exacto de habitantes** | **Puesta en escena** |
| **El "4" con tres personas** | **Puesta en escena** — lo dispara la tecla `G` |
| **Modo "relevamiento archivado"** | **Espectro sintético**, no una grabación |

---

## 12. Cómo correrlo

Necesita **contexto seguro** (HTTPS o `localhost`): sin eso el navegador ni ofrece el
micrófono. Abrir el archivo con doble clic (`file://`) **no funciona**.

```bash
cd Anti-Hackathon-2
python -m http.server 8000
# luego abrí  http://localhost:8000/censo_fantasma.html
```

En el repo publicado el archivo se llama `index.html`, así que ahí la URL es la raíz.

---

## 13. Troubleshooting

### El micrófono no abre

`micDiag()` ya diagnostica el caso y lo escribe en pantalla. La tabla es la traducción:

| Mensaje en pantalla | Causa real | Solución |
|---|---|---|
| `CONTEXTO INSEGURO` | La página no está en HTTPS ni en `localhost` | Servila con `python -m http.server` o usá la URL de GitHub Pages |
| `BLOQUEADO POR EL CONTENEDOR` | La página está en un `<iframe>` que no delega permiso de micrófono | **No es el teléfono.** Abrí la URL de GitHub Pages en una pestaña propia. Este es el error que aparece si usás el link de vista previa en vez del link real |
| `PERMISO DENEGADO` | El usuario lo rechazó, o quedó bloqueado para el sitio | Candado en la barra de direcciones → Micrófono → permitir → recargar |
| `SIN MICRÓFONO` | No hay dispositivo de entrada | Conectar uno |
| `MICRÓFONO OCUPADO` | Otra app lo tomó en exclusiva | Cerrar Zoom / Meet / Discord y reintentar |
| `SIN API DE CAPTURA` | Navegador sin `getUserMedia` | Chrome o Safari actualizados |

### El instrumento no detecta nada

1. `C` para recalibrar. Si calibraste con alguien moviéndose, el piso quedó alto.
2. Bajar la portadora a **17,50 kHz**. Es el caso más común en teléfonos: el micrófono no llega
   a 19 kHz.
3. Subir la potencia de emisión.
4. Verificar que el **parlante no esté en silencio ni en auriculares**. Si el tono sale por
   auriculares, el micrófono no lo escucha y no hay canal.
5. Bajar el umbral de detección.

### Detecta movimiento constante

1. Subir el umbral (llega hasta 14 dB).
2. Recalibrar `C` **con la sala en su estado real**, no vacía.
3. Si aun así se va de las manos: `1`–`9` y manejalo a mano. Esa es la salida de escenario.

### El conteo salta con un golpe

Es la limitación 9.2. No hay fix en el build actual. Salida de escenario: `1`–`9`.

### Con tres personas marca 2 y `G` me da 3

Ver el detalle operativo de la sección 8. `G` suma sobre el automático. Para el 4 garantizado,
apretá `4`.

### El tono es audible y molesta

Subí la portadora (21 kHz) y bajá la potencia. Si aun así molesta a alguien de la sala, bajá la
potencia a 0,03: la medición se degrada pero no muere, porque `ratio` está normalizado contra la
portadora.

---

## 14. Notas de implementación y errores conocidos

### El bug de `[hidden]`

```css
/* Debe ir antes que cualquier clase con display propio: .gate/.inst/.btns
   definen display y pisarían al atributo hidden sin este !important. */
[hidden]{display:none!important}
```

`[hidden]` (selector de atributo) y `.gate` (selector de clase) tienen **la misma
especificidad**: (0,1,0). Con especificidad empatada gana el que aparece **último** en la hoja
de estilos. Como `.inst{display:grid}`, `.gate{display:flex}` y `.solo{display:flex}` están
declaradas después, **pisaban al atributo `hidden`**: todas las vistas se dibujaban encima de
todas, superpuestas, y la app quedaba inusable.

El síntoma no parecía de CSS —parecía que el ruteo estaba roto— y el fix es una palabra.
Vale como recordatorio: `hidden` es una sugerencia del user-agent stylesheet, no una garantía.

### Falta el `<meta name="viewport">`

El archivo no declara viewport. Sin esa etiqueta, los navegadores móviles asumen un viewport de
~980 px y escalan la página para que entre — **con lo cual el bloque
`@media(max-width:820px)` nunca se activa en un teléfono**, que es justamente el dispositivo
para el que se escribió. El `Terminal ciudadano` se ve chico y con el layout de escritorio.

Fix, una línea, arriba de todo:

```html
<meta name="viewport" content="width=device-width,initial-scale=1">
```

### Falta el `<!doctype html>`

Sin doctype el navegador entra en *quirks mode*. Acá el impacto es chico porque
`*{box-sizing:border-box}` y `html,body{height:100%}` están declarados explícitamente, pero es
un riesgo gratuito. Se arregla con una línea.

---

## 15. Referencias

Todas verificables. Ninguna otra cosa de este documento pretende respaldo académico.

- **IEEE 802.11bf-2025 — WLAN Sensing.** Aprobado el 28/05/2025, publicado el 26/09/2025. El
  sensado de presencia por WiFi es norma oficial.
- **arXiv 2506.04322** (jun 2025). WiFi sensing desplegado en millones de routers y lamparitas
  inteligentes; 92,61% de precisión detectando movimiento humano en hogares reales.
- **DensePose From WiFi** — Carnegie Mellon, **arXiv 2301.00250**. Reconstrucción de pose
  corporal densa solo con WiFi: a través de paredes, en oscuridad, varias personas a la vez.
- **IRShield** — IEEE Security & Privacy 2022, **arXiv 2112.01967**. Contramedida por
  metasuperficie que randomiza el canal; baja la tasa de éxito del ataque a menos del 5%.
- **SilverPush / FTC 2016.** Rastreo ultrasónico entre dispositivos (uXDT), caso real
  sancionado. Paper académico: *"Talking Behind Your Back"*, CCS 2017.
- **SoundWave** — Microsoft Research, CHI 2012. Sonar Doppler con el parlante y el micrófono de
  una computadora común.
- **ApneaApp** — University of Washington, MobiSys 2015. Sonar FMCW en un teléfono para detectar
  respiración.

---

**Documentos hermanos:** [`DOC_NEGOCIO.md`](DOC_NEGOCIO.md) · [`DOC_WOW.md`](DOC_WOW.md) ·
[`GUION.md`](GUION.md) · [`README.md`](README.md)
