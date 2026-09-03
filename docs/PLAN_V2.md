# CENSO FANTASMA v2 — Plan de trabajo

Estado base: consola Doppler funcionando en https://justuris.github.io/censo-fantasma/
Confirmado por el Creador: detecta movimiento. Falla: cuenta 4 personas habiendo 1.

---

## Los tres problemas reales

### P1 · Falsos positivos por transitorio acústico
**Síntoma:** golpeás la mesa → detecta 4 habitantes.
**Causa:** un golpe es **banda ancha** — inyecta energía en todo el espectro, incluidas las
bandas laterales que yo leo como Doppler. Mi detector mide energía lateral y no pregunta
de dónde viene.
**Fix:** canal de referencia. Un reflejo Doppler humano vive en **±15…±220 Hz** de la portadora
(porque `Δf = 2·v·f₀/c` y un cuerpo no supera ~1,9 m/s). Un golpe sube **todo**, incluido
±800…±1500 Hz, donde ningún cuerpo humano puede producir señal.

```
si (referencia_lejana sube junto con las bandas laterales) → transitorio acústico → descartar
si (solo suben las bandas laterales)                       → Doppler real       → contar
```

Esto es rechazo de modo común. Es la solución correcta, no un umbral más alto.

### P2 · El conteo salta
**Causa:** el conteo sale de un pico instantáneo con decaimiento. Cualquier golpe lo dispara al techo.
**Fix:** ocupación **sostenida**. Subir requiere N cuadros consecutivos por encima del umbral;
bajar es lento. Es como funciona un sensor de ocupación real.

### P3 · "Recrear la escena" — dónde estamos parados de verdad
**La verdad, sin adorno:** el Doppler te dice *que algo se mueve* y *hacia dónde*. No te dice
*dónde está*. Reconstruir una habitación en 2D necesita **arreglo de micrófonos**; el cuerpo
calcado necesita **CSI de WiFi + red entrenada**. Ninguna de las dos pasa esta noche.

**Lo que sí llega esta noche: FMCW.** Y llega lejos.

---

## FMCW — el módulo nuevo

En vez de un tono fijo, se emite un **barrido lineal 17→22 kHz** que se repite. Se correlaciona
lo que vuelve contra la plantilla del barrido. Eso ya no es "hay movimiento": es **distancia**.

```
Resolución de rango   = c / (2·B)      = 343 / (2·5000)   = 3,4 cm
Paso por muestra      = c / (2·fs)     = 343 / (2·48000)  = 3,57 mm
Alcance útil          ≈ 2,8 m   (800 retardos)
Período de barrido    = 1024 muestras  = 21,3 ms  → 47 barridos/s
```

**Cómo se vuelve una escena:**
1. Correlación → perfil de rango (energía reflejada vs distancia).
2. El pico más fuerte es el **camino directo** parlante→micrófono. Es el cero de la regla.
3. Se promedia el perfil en el tiempo → eso es el **clutter estático**: paredes, mesa, monitor.
4. **Se resta.** Lo que queda es lo que se mueve.
5. Se apila en el tiempo → **cascada rango-tiempo**: se ve la traza del cuerpo acercándose.

Eso es un radar. Es real, es honesto y es lo más cerca de "recrear la escena" que existe
sin hardware. Es 1D (distancia), no 2D (planta). **Decirlo en escenario suma, no resta.**

### Riesgo
La correlación cuesta ~500k multiplicaciones por cuadro. En laptop vuela. En teléfono puede
arrastrarse → **FMCW queda solo en la Consola**, que es donde se usa igual.

---

## Además: "dado que esto ya es real, entonces necesitamos…"

| Módulo | Por qué existe en ese mundo | ROI |
|---|---|---|
| **Certificado de No Existencia** | Si el Estado releva por canal físico, hace falta un instrumento que acredite ausencia. PDF sellado, con hash. **El público se lo lleva y lo comparte.** | 🔥 Alto — es el souvenir |
| **Padrón de Habitantes No Declarados** | Todo censo genera un padrón. Este lista los cuerpos detectados que nadie declaró. | Alto — es el remate del "4" |
| **Derecho a la Ausencia** | El marco legal que ese mundo necesitaría. Es el documento de negocio. | Medio — le da peso |

---

## Orden de ejecución

1. Fix P1 + P2 (credibilidad — sin esto el "4" no tiene gracia porque la máquina parece random)
2. FMCW (la magia)
3. Certificado de No Existencia (el souvenir)
4. Documentación completa (negocio, tecnología, WOW humano, WOW técnico)
5. Actualizar guion

## Preguntas abiertas — no bloquean, seguí sin contestar

- ¿Proyectás desde la misma laptop que corre el sensor? Cambia dónde ponés el micrófono.
- ¿Tenés un segundo teléfono para el Velo? Si no, el Velo se activa con tecla `V` en la consola.
