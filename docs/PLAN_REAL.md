# CENSO FANTASMA — plan para que sea real

> La hackathon pasó. Se terminó el teatro.
> **Vara nueva:** cada fase tiene un criterio de aceptación medible. Si no pasa la prueba,
> no está hecha. Nada de "más o menos anda".

---

## 1. Qué está roto hoy — sin maquillaje

| # | Defecto | Gravedad |
|---|---|---|
| D1 | **El conteo de habitantes es puesta en escena.** Sale de las teclas `1`-`9`, no de la señal. | 🔴 Es la mentira central |
| D2 | **El ESP32 nunca funcionó de punta a punta.** Las placas dejaron de enumerar en USB. | 🔴 Bloquea todo lo bueno |
| D3 | **El radar de rango es inestable.** Suavizado a último momento, nunca validado contra una cinta métrica. | 🟠 |
| D4 | **La detección acústica es binaria y cruda.** Umbral sobre energía. Sin teoría de detección, sin tasa de falsos positivos conocida. | 🟠 |
| D5 | **No hay ubicación.** Solo "hay alguien". | 🟡 |
| D6 | **Cero validación.** Ningún número del sistema fue nunca contrastado contra la realidad. | 🔴 Es lo que separa un juguete de un instrumento |

**El más grave es D6.** Todo lo demás se arregla; sin medición contra verdad de campo no sabés
si arreglaste algo.

---

## 2. Dónde está la magia de verdad

No en detectar movimiento — eso lo hace un sensor de $2. La magia real, en orden:

### 🥇 Respiración a través de una pared
*"No se está moviendo. Está quieto. Pero respira. Y eso alcanza."*

Es lo más fuerte que se puede construir, y **es alcanzable**. Publicado y reproducido
(ApneaApp, UW MobiSys 2015 — sonar de celular; y toda la línea de Wi-Fi respiration sensing).

**Por qué es alcanzable y no lo era antes:** el tórax se mueve ~5 mm. A 19 kHz la longitud de
onda es 18 mm. Ida y vuelta, 5 mm de desplazamiento son **~200° de cambio de fase**. Enorme.
Perfectamente medible.

**El detalle que lo destraba:** mi radar FMCW ya calcula I y Q — y después tira la fase para
quedarse con la magnitud. **La fase ya está ahí. Solo hay que dejar de descartarla.**

### 🥈 Atravesar la pared de verdad
El techo duro del canal acústico: el sonido no pasa. El WiFi sí. Necesita CSI andando.

### 🥉 Ubicación real
Dos o tres receptoras → dónde está, no solo que está.

### Silueta / postura
Es DensePose. Necesita red entrenada y dataset propio con cámara como verdad de campo.
**Semanas, no horas.** Hay camino, pero no es el próximo paso.

---

## 3. El plan

### FASE 0 · Verdad de campo *(sin esto, nada)*
El sistema tiene que poder compararse contra la realidad.

- Registrador: guarda a disco todas las mediciones crudas con marca de tiempo.
- Marcador manual: una tecla para anotar "acá entré", "acá salí", "acá había 2".
- Reproductor: correr el procesamiento sobre una sesión grabada, sin hardware.

**Aceptación:** poder grabar 10 minutos, cambiar el algoritmo, y **re-correrlo sobre los mismos
datos** para comparar. Sin esto, cada cambio es fe.

**Sin hardware. Se puede hacer ya.**

---

### FASE 1 · Respiración acústica ← *el salto de calidad*

1. Conservar el número complejo del filtro adaptado en cada bin de rango (hoy se descarta).
2. Seguir el bin donde está el cuerpo.
3. Desenvolver la fase en el tiempo → desplazamiento en milímetros.
4. Pasabanda 0,1–0,6 Hz (6 a 36 respiraciones por minuto).
5. FFT sobre ventana de 30 s → frecuencia respiratoria.

**Aceptación:** sentado inmóvil a 1 m, el sistema reporta tu frecuencia respiratoria
**dentro de ±2 por minuto** contra conteo manual, en 3 pruebas de 60 s seguidas.

**Sin hardware. Es lo próximo que construyo.**

---

### FASE 2 · Medición honesta *(matar D1 y D4)*

- Sacar el conteo de las teclas. El número sale de la señal o no sale.
- Detección estadística de verdad: modelar el ruido de fondo y disparar por probabilidad,
  no por umbral fijo.
- **Publicar la tasa de falsos positivos** en la propia pantalla.
- Distinguir 0 / 1 / "más de uno" con honestidad. Si no se puede distinguir 3 de 4, decirlo.

**Aceptación:** en 20 minutos con la habitación vacía, **menos de 1 falso positivo**.
Y detectar a una persona entrando en menos de 3 segundos, 10 de 10 veces.

---

### FASE 3 · Desbloquear el ESP32 *(matar D2)*

Diagnóstico ordenado, una variable por vez:

1. Probar **cada cable** con la placa que ya funcionó. Los de solo carga se ven idénticos.
2. Probar **cada puerto USB** directo, sin hub.
3. Probar **cada placa** con el cable bueno confirmado.
4. Anotar qué combinación funciona. Es una tabla de 4×4, no adivinanza.

**Aceptación:** dos placas enumerando a la vez, con CSI de 64 subportadoras a más de 50 Hz,
durante 10 minutos sin cortes.

---

### FASE 4 · Respiración por WiFi, a través de la pared
Lo mismo que la Fase 1 pero sobre la fase del CSI. Acá el sonido no llega y el WiFi sí.

**Aceptación:** frecuencia respiratoria de alguien **del otro lado de una pared de material**,
±3 por minuto contra conteo manual.

Si esto sale, tenés algo que casi nadie tiene hecho fuera de un laboratorio.

---

### FASE 5 · Ubicación
Una emisora, dos o tres receptoras. Perturbación relativa por enlace → zona.

**Aceptación:** con la sala dividida en 4 zonas, acertar la zona **8 de 10 veces**.

---

### FASE 6 · Silueta *(proyecto aparte, semanas)*
Dataset propio: cámara web como verdad de campo, CSI sincronizado, red entrenada.
No arranca hasta que las fases 3 y 4 estén sólidas.

---

## 4. Orden de ejecución

```
AHORA, sin hardware:   Fase 0  →  Fase 1  →  Fase 2
CUANDO HAYA HARDWARE:  Fase 3  →  Fase 4  →  Fase 5
DESPUÉS, si hay ganas: Fase 6
```

**Arranco por Fase 0 y Fase 1.** La respiración acústica es el mayor salto de magia por hora
invertida, no necesita comprar nada, y no depende de que aparezca ninguna placa.

---

## 5. Cómo se ve el éxito

No es una pantalla más linda. Es poder decir, con un número atrás:

> *"Está sentado del otro lado de esa pared. No se mueve.
> Respira catorce veces por minuto.
> Y no tiene nada encima."*

Y que sea **verdad**, medible y repetible.
