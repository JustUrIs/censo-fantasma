# Cómo probar esto — guía sin vueltas

**App:** https://justuris.github.io/censo-fantasma/

---

## Lo primero: hacé el MÓDULO 09 · LABORATORIO

No adivines. Entrá al **Laboratorio** y tocá **EMPEZAR**.

Te va diciendo en pantalla grande qué hacer, con cuenta regresiva. Dura 4 minutos.
Al final tocás **Descargar sesión** y me pasás el archivo.

**Con ese archivo yo puedo decirte si funciona o no.** Sin él, los dos estamos adivinando.

Los 7 pasos son: salir de la pieza → entrar y quedarte parado → caminar → sentarte
quieto un minuto → contar tus respiraciones → alejarte 3 metros → salir otra vez.

Mientras tanto podés tocar los botones **"Estoy quieto" / "Me muevo" / "1 persona"** cuando
quieras. Cada toque queda guardado con la hora exacta.

---

## Tus preguntas, una por una

### ¿Tengo que estar solo?
**No.** Pero si hay alguien más moviéndose, el equipo lo va a ver también y no sabe cuál sos vos.
Para *probar si funciona*, mejor solo. Para *usarlo*, da igual.

### ¿Puede haber gente al lado?
Sí. Solo que van a aparecer en la medición.

### ¿Los dispositivos cerca molestan?
**No.** Ni el router, ni el celular, ni la tele. Esto es **sonido**, no radio.
El WiFi no lo afecta en absoluto.

### ¿Me tengo que sacar la ropa?
**No.** La ropa no cambia nada — el sonido rebota igual en un buzo que en una remera.

### ¿A qué distancia me pongo?
**Entre 0,5 y 2 metros.** El alcance máximo es 2 metros.
Más lejos que eso, no te ve — no es que funcione mal, es el límite físico.

### ¿Qué SÍ arruina la medición?
Esto es lo único que importa, y es corto:

| Arruina | Por qué |
|---|---|
| **Ventilador o aire acondicionado** | Mueve aire todo el tiempo = movimiento permanente |
| **Música o tele prendida** | Mete ruido en la banda que escuchamos |
| **Ventana abierta a la calle** | Ruido de afuera |
| **La computadora en tus piernas** | Cada micro-movimiento tuyo mueve el sensor entero |

**Ponela sobre una mesa firme y apagá el ventilador.** Con eso ya está.

---

## Por qué "estoy adelante y no me detecta"

Esto era un bug de uso, y era culpa mía por no explicarlo en pantalla. **Ya lo arreglé.**

El equipo, al arrancar, **aprende cómo suena la sala**. Toma una foto del silencio.

Si cuando arranca vos ya estabas sentado ahí, **vos pasás a ser parte de esa foto**.
Te vuelve invisible: para él, "vos ahí sentado" es lo normal.

**Ahora la pantalla te lo dice:**
1. `NO TE MUEVAS · APRENDIENDO CÓMO SUENA LA SALA` ← quedate quieto 3 segundos
2. `LISTO · AHORA MOVETE PARA PROBAR` ← ahí sí, movete

Si en cualquier momento se pierde, apretá **`C`** y volvés a empezar la calibración.

### Y lo otro importante
**El detector de presencia solo ve MOVIMIENTO.** Si te quedás totalmente quieto, desaparecés.
No está roto: es cómo funciona.

Para detectar a alguien **quieto** está el **Módulo 08 · Respiración**. Ese sí ve un cuerpo
inmóvil, porque busca el movimiento del pecho al respirar.

---

## Los números que cambian demasiado rápido

Tenías razón, era ilegible. Ya lo arreglé: ahora el número se refresca **3 veces por segundo**
en vez de 14, y se mantiene 2 segundos aunque pierda la señal.

Si igual salta mucho, es que hay algo moviéndose en la sala (ventilador, cortina, otra persona).

---

## Cuando conectes los ESP32 — explicado desde cero

### Qué es cada placa
Son cuatro plaquitas iguales, pero **cada una tiene un programa distinto adentro**:

| Placa | Qué hace | Dónde se enchufa |
|---|---|---|
| **Sniffer** | Cuenta cuántos aparatos hay en la sala | **A la compu**, con cable de datos |
| **Emisora** | Grita "hola" 200 veces por segundo | **A cualquier lado que dé corriente** |
| **Receptora** | Escucha a la emisora y mide cómo llega | **A la compu**, con cable de datos |

### La regla más importante

> **Solo las que van a la compu necesitan cable de datos.
> La emisora solo necesita corriente.**

La emisora podés enchufarla al cargador del celular, a un power bank, a una zapatilla USB.
No le importa. Solo quiere electricidad.

### El cable — acá falla todo el mundo

Hay dos tipos de cable USB y **se ven exactamente iguales**:

- **De carga:** solo lleva electricidad. La placa prende, pero **la compu no la ve.**
- **De datos:** lleva electricidad *y* información. Este es el que sirve.

**Cómo saber cuál tenés:** enchufá la placa y corré esto:

```powershell
cd "C:\Users\justu\Downloads\Anti-Hackathon-2\firmware"
.\que_es.ps1
```

- Si te dice qué placa es → **cable de datos** ✅
- Si dice "no hay ninguna placa" → **cable de carga** ❌ (o el puerto está fallado)

**Probá todos tus cables así, uno por uno, y marcá los buenos con una cinta.**
Es lo que te va a ahorrar más tiempo de todo.

### ¿Puedo enchufar todas a la misma compu?

**Sí, técnicamente.** Pero ayer no te funcionó, y las razones probables son:

1. **Cables de carga** en las placas nuevas ← lo más probable
2. **Un hub USB** — muchos no pasan datos bien, o no dan corriente para 4 placas
3. Un puerto USB de la compu que está fallado

**Probá siempre así: UNA placa, DIRECTO a la compu (sin hub), con un cable ya confirmado.**
Recién cuando esa anda, agregá la segunda.

### Dónde poner cada una, físicamente

```
   [EMISORA]  ← en un rincón, con power bank o cargador
       |
       |   ← acá camina la persona
       |
   [RECEPTORA] ← en otro rincón, con cable de datos a la compu
```

**La persona tiene que pasar ENTRE las dos.** Ahí es donde la señal se deforma más.
Si las ponés juntas al lado de la compu, casi no vas a ver nada.

Para atravesar una pared: **emisora de un lado, receptora del otro.**

---

## Resumen de lo que te pido

1. Entrá al **Módulo 09 · Laboratorio**
2. Apagá ventilador y música
3. Tocá **EMPEZAR** y seguí los pasos
4. Al final, **Descargar sesión** y me pasás el archivo

Con ese archivo dejo de adivinar y te digo con números si funciona, qué tan bien,
y qué hay que arreglar.
