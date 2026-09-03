# La magia — qué se puede mostrar de verdad con 4 ESP32

> Sala: ~50 personas, todas con celular, la mitad con laptop, público técnico.
> Hardware: 4 × NodeMCU ESP32-WROOM (micro-USB).
> Regla: **el demo acústico ya funciona.** Todo esto es aditivo. Nada de lo de acá abajo
> puede tocar el camino que ya anda.

---

## Por qué el ESP32 cambia la categoría del proyecto

Hoy tu charla dice: *"el WiFi puede hacer esto, mirá el equivalente acústico"*.
Es una analogía honesta pero **tiene un techo**: el sonido de 19 kHz **no atraviesa una pared**.

El WiFi sí.

Ese es el salto. No es "el mismo demo pero mejor": es **el demo que el acústico no puede hacer**,
y es exactamente lo que el público técnico va a querer ver.

---

## 🥇 MAGIA 1 — EL CENSO DE DISPOSITIVOS
### *"Nadie en esta sala se conectó a nada mío. Igual los tengo contados a todos."*

**Riesgo: MUY BAJO · Impacto: MÁXIMO · Necesita: 1 placa · Sin CSI**

Esta es la que yo pondría primera, y es la que menos puede fallar.

**Qué es:** el ESP32 en **modo promiscuo** escucha todo el tráfico 802.11 del aire. Cada
celular emite *probe requests* buscando redes conocidas, permanentemente, aunque no esté
conectado a nada. Contás direcciones únicas y tenés el censo de la sala.

**El momento:**

> "No les pedí que se conecten a nada. No hay QR. No instalaron nada.
> Este aparato de cinco dólares lleva prendido desde que subí."

*(Aparece el contador subiendo en vivo: 40… 60… 90…)*

> "Noventa y cuatro dispositivos.
> Somos cincuenta personas."

*(Pausa.)*

> "El censo de 2022 preguntó cuántos duermen acá.
> Este preguntó cuántos **están**. Y no preguntó."

**Por qué es tan fuerte:** es el único demo donde **el público entero es el sujeto medido sin
haber hecho nada**. No hay voluntario, no hay panel: son ellos.

**El giro técnico que la hace todavía mejor.** Los teléfonos modernos **aleatorizan su MAC**
justo para evitar esto. Así que el número va a estar inflado — un mismo teléfono aparece
varias veces. **Eso no es un bug, es el mejor dato de la noche:**

> "El número está inflado. Sus teléfonos mienten sobre su identidad.
> Eso es una **contramedida**, y viene activada de fábrica desde iOS 14 y Android 10.
>
> O sea: la industria ya sabía que esto se podía hacer. Y no les avisó — les puso el parche
> y siguió de largo."

**⚠️ Cómo se construye responsablemente.** La MAC se **hashea en el momento** de leerla, nunca
se muestra, nunca se guarda, nunca sale de la placa en claro. Solo se cuentan hashes únicos en
una ventana de tiempo. Es una decisión de diseño y **decila en escenario**:

> "Las direcciones las hasheo al vuelo y no guardo ninguna.
> Que es más de lo que hace el censo de verdad."

---

## 🥈 MAGIA 2 — A TRAVÉS DE LA PARED
### *"El sonido no pasa. Esto sí."*

**Riesgo: MEDIO · Impacto: MÁXIMO · Necesita: 2 placas · Con CSI**

**Qué es:** el corazón de la charla. Ponés el emisor de un lado de una pared real —no del panel
de cartón, de una **pared**— y el receptor del otro. Una persona camina del lado del emisor.
La consola acústica no ve nada. La de WiFi la detecta.

**El momento, que es de dos tiempos:**

1. Primero lo hacés con el sensor acústico y **falla**. A propósito.
   > "El sonido de diecinueve mil hertz no atraviesa la pared. Rebota. Acá se termina el sonido."
2. Cambiás a WiFi.
   > "Pero el WiFi de tu casa atraviesa la pared todos los días. Es literalmente para eso.
   > Y si atraviesa, vuelve deformado por lo que hay del otro lado."

*(La traza salta.)*

> "Hay alguien del otro lado de esa pared. Y no sabe que lo estamos midiendo."

**Por qué funciona dramatúrgicamente:** mostrar primero el fracaso hace que el éxito valga
diez veces más. Y te blinda contra la objeción más probable del público técnico, porque **la
respondés vos antes de que la hagan**.

**Riesgo real:** hay que encontrar la pared. Sirve el baño, un pasillo, una puerta cerrada,
la parte de atrás del escenario. Hay que probarlo en el lugar.

---

## 🥉 MAGIA 3 — LA CASCADA DE SUBPORTADORAS
### *"Esto es el canal. Esto es lo que tu router ve de vos."*

**Riesgo: BAJO · Impacto: ALTO · Necesita: 2 placas · Con CSI**

**Qué es:** el ESP32 no da un número, da **64 subportadoras** — el canal medido en 64 frecuencias
a la vez, decenas de veces por segundo. Dibujado como cascada, es una imagen que respira.

Cuando alguien camina entre las dos placas, **las 64 líneas se retuercen juntas**, con estructura,
no como ruido. Es de las cosas más hipnóticas que se pueden proyectar.

**El momento:**

> "Esto no es una animación. Son sesenta y cuatro frecuencias midiendo esta sala,
> cien veces por segundo. Cada línea es una subportadora de WiFi.
>
> Miren lo que pasa cuando alguien camina."

**Por qué la incluyo alto:** es la que **más rinde por riesgo asumido**. Una vez que el CSI
llega al navegador, esto es solo dibujar. Y a un público técnico le habla directo: están viendo
la matriz cruda, no una interpretación tuya.

---

## MAGIA 4 — EL EMISOR SUELTO
### *"La sala entera es el sensor."*

**Riesgo: BAJO · Impacto: MEDIO-ALTO · Necesita: 2 placas + power bank**

El emisor no necesita datos, **solo corriente**. Lo enchufás a un power bank y **se lo das a
alguien del público**. Que camine por la sala con el aparato en el bolsillo.

El enlace entre esa persona y la placa receptora atraviesa a todos los demás. La señal se
deforma según por dónde camine y a quién tenga cerca.

> "El sensor ya no está en el escenario. Está caminando entre ustedes.
> Y todos los que están en el medio son parte de la medición."

**Extra:** es el demo que mejor se filma. Alguien caminando entre la gente mientras la pantalla
responde es la toma que después se comparte.

---

## MAGIA 5 — LA RESPIRACIÓN
### *"No se está moviendo. Pero respira. Y eso alcanza."*

**Riesgo: ALTO · Impacto: DEVASTADOR · Necesita: 2 placas + sala en silencio**

Publicado y reproducido muchas veces. El tórax se mueve ~5 mm a 0,2–0,4 Hz, y eso **sí** se ve
en la fase del CSI si el enlace está quieto.

**El momento** (y el pedido a la sala es parte del truco):

> "Necesito treinta segundos de silencio absoluto. Nadie se mueve. Ni un pie.
> Ustedes también son parte del canal."

*(Cincuenta personas congeladas. Eso solo ya es un momento.)*

> "El voluntario está inmóvil detrás del panel. El detector Doppler no lo ve:
> sin velocidad no hay corrimiento.
>
> Pero mírenlo."

*(Aparece la onda respiratoria.)*

> "Catorce por minuto. Está respirando.
> Y eso alcanza para saber que hay un cuerpo ahí."

**Por qué es riesgo alto:** cincuenta personas en una sala son cincuenta fuentes de movimiento.
Aunque se queden quietas, hay deriva. Necesita el enlace corto (1–2 m), el cuerpo bien en el
medio, y ~30 s limpios. **Es la que más chance tiene de no salir.**

**Cómo blindarla:** grabala en tu casa esta tarde, con la sala vacía y en silencio. Si sale en
vivo, gloria. Si no, ponés la grabación y decís *"esto lo medí hace tres horas en mi
habitación"* — que también es verdad y también impresiona.

---

## MAGIA 6 — DÓNDE ESTÁ, NO SOLO QUE ESTÁ
### *"Zona 3."*

**Riesgo: ALTO · Impacto: ALTO · Necesita las 4 placas**

Con **1 emisor + 3 receptores** repartidos por la sala tenés tres miradas distintas del mismo
volumen. El receptor cuya señal más se perturba es el más cercano al cuerpo.

No es un plano de la habitación —eso necesita arreglo de antenas— pero sí un **mapa de zonas**
que se ilumina donde está la persona. Visualmente es "recrear la escena", que es lo que venías
buscando.

**Honestidad:** es localización por zonas, no por coordenadas. Decilo así y suma.

**Por qué queda sexto:** consume las 4 placas, 3 puertos USB y es lo que más tiempo de
calibración pide. **Solo si todo lo demás ya está cerrado.**

---

## Lo que NO se puede hoy (para que no lo prometas)

| Demo | Por qué no |
|---|---|
| **Cuerpo calcado (DensePose From WiFi)** | Necesita red entrenada + dataset + GPU. El paper existe, el checkpoint no se publica. **Contalo, no lo prometas.** |
| **Teclas tipeadas (WiKey)** | Requiere entrenamiento por teclado y por persona. Semanas. |
| **Identificar personas por la forma de caminar** | Igual: entrenamiento por sujeto. |
| **Ver a través de una pared de hormigón** | Drywall y madera sí. Hormigón con malla metálica, no. **Probá la pared antes.** |

---

## ORDEN DE CONSTRUCCIÓN

Cada bloque termina en algo presentable. Si se corta el tiempo, se corta acá y lo anterior sirve.

| # | Qué | Placas | Riesgo | Corte |
|---|---|---|---|---|
| **B1** | Firmware sniffer + lector Web Serial + **Censo de dispositivos** | 1 | 🟢 | Si esto anda, ya ganaste algo que el acústico no podía |
| **B2** | Firmware CSI (emisor + receptor) + **cascada de 64 subportadoras** | 2 | 🟡 | La imagen que se proyecta |
| **B3** | **Detección a través de la pared** sobre B2 | 2 | 🟡 | El clímax de la charla |
| **B4** | **Emisor suelto** con power bank | 2 | 🟢 | Casi gratis una vez que B2 anda |
| **B5** | **Respiración** — grabar en casa como respaldo | 2 | 🔴 | Solo si B2 quedó sólido |
| **B6** | **Zonas** con 3 receptores | 4 | 🔴 | Solo si sobra todo |

### Puntos de control duros

- **Si B1 no está andando en 60 minutos** → se abandona todo el ESP32 y se presenta lo acústico.
- **Si B2 no está andando 3 horas antes de presentar** → se congela en B1 y se ensaya.
- **El ensayo en voz alta no se negocia.** Vale más que B5 y B6 juntos.

### Lo que hacés vos mientras yo escribo

1. Instalar **Arduino IDE** y agregar el soporte de placas ESP32
   (Preferencias → URL de gestor de tarjetas → `https://espressif.github.io/arduino-esp32/package_esp32_index.json`,
   después Gestor de tarjetas → buscar "esp32" → instalar).
2. Enchufar una placa y confirmar que **aparece un puerto COM nuevo**.
   Si no aparece: driver **CP2102** (Silicon Labs) o **CH340**.
3. Cargar el ejemplo `Blink` para verificar que flashea. Placa: **"ESP32 Dev Module"**.

Cuando el LED parpadee, avisame y te paso el firmware.
