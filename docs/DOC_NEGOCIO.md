# Dirección Nacional de Teledetección Habitacional

### Memoria institucional del Programa de Relevamiento Pasivo de Ocupación

**EXP. 17622/26 — FORM. C-4, ANEXO II**
Instituto Nacional de Estadística y Censos · Sistema Estadístico Nacional

---

> **Nota de encuadre — leer antes de seguir.**
> La **Dirección Nacional de Teledetección Habitacional no existe**. Es la ficción del
> proyecto *Censo Fantasma*, presentado en la anti-hackathon *SideQuest — Conspiracy Edition*.
> El expediente, el formulario y las resoluciones citadas son ficticios.
> **La Ley 17.622 es real**: sancionada en 1968, creó el INDEC y el Sistema Estadístico
> Nacional, y su artículo 10 establece el secreto estadístico. Las referencias técnicas y
> académicas del final también son reales y verificables.
> De acá en adelante, el documento está escrito **desde adentro del organismo**. Ese es el punto.

---

## 1. Objeto

La Dirección Nacional de Teledetección Habitacional tiene por objeto el **relevamiento de
ocupación efectiva de la vivienda por variación del canal físico**, sin intervención declarativa
del habitante y sin requerimiento de acceso a dispositivo, cámara, ubicación ni aplicación
instalada.

El principio es único y no admite excepción material:

> Toda onda que atraviesa un ambiente vuelve deformada por los cuerpos que hay adentro.

De esa deformación se deriva el dato. El habitante no participa del acto de relevamiento. No
puede participar: el fenómeno es anterior a su voluntad.

---

## 2. El problema que el organismo resuelve

El instrumento declarativo tiene tres fallas estructurales que ningún operativo de campo
corrigió nunca, porque no son corregibles por vía declarativa:

**2.1 — La vivienda cerrada.** El censista que no encuentra a nadie no releva. Vuelve, o no
vuelve. El dato de la vivienda cerrada se imputa, es decir, **se inventa por modelo**. El
organismo asume, sin eufemismo, que una fracción del padrón histórico es estimación.

**2.2 — La declaración interesada.** La pregunta *"¿cuántas personas durmieron anoche en esta
vivienda?"* se responde en función de lo que el declarante cree que se hace con la respuesta.
Alquiler informal, subalquiler, convivencia no registrada, situación migratoria irregular,
guarda de menores en disputa: todas producen subdeclaración sistemática, no aleatoria. El error
declarativo tiene **dirección**. Un error con dirección no se corrige agrandando la muestra.

**2.3 — El costo y la frecuencia.** El operativo censal presencial es el operativo logístico más
caro del Estado y se ejecuta una vez por década. Entre censo y censo, el organismo opera sobre
una fotografía que envejece.

**El relevamiento pasivo resuelve las tres.** No requiere que abran la puerta. No requiere que
contesten. No requiere que digan la verdad. Y **es continuo**: no hay un día del censo, hay un
estado del censo.

---

## 3. Marco normativo

### 3.1 Norma habilitante

La **Ley 17.622** crea el INDEC y el Sistema Estadístico Nacional, y le asigna la conducción de
la actividad estadística oficial. La ley no enumera medios de captación: **encomienda un fin y
guarda silencio sobre la técnica**. El silencio de la norma respecto del medio es, en la doctrina
interna de esta Dirección, habilitación suficiente.

### 3.2 El artículo 10 y el argumento del organismo

El **artículo 10** establece el **secreto estadístico**: la información recogida es
estrictamente confidencial, se utiliza únicamente con fines estadísticos y no puede divulgarse
con referencia a persona o entidad determinada.

Esta Dirección sostiene que **el relevamiento pasivo no compromete el artículo 10, y que además
lo cumple mejor que el formulario**, por tres razones:

1. **No hay dato personal en origen.** Lo que se registra no es una persona: es una magnitud
   física del ambiente — energía en una banda lateral, expresada en decibeles sobre un piso
   calibrado. Nunca hubo un nombre que anonimizar.
2. **No hay audio.** El equipo no graba, no transmite y no almacena sonido. Aplica una
   transformada al flujo de entrada, lee una banda estrecha, descarta el resto y no persiste
   nada. El dato **nace agregado**.
3. **No hay declarante que pueda ser identificado con su declaración**, porque no hay
   declaración.

### 3.3 La objeción que el organismo reconoce

La Dirección deja constancia, para el expediente, de que este razonamiento **tiene un punto
débil y que el punto débil es serio**:

> El artículo 10 protege *lo que la persona declara*. No fue escrito para el caso en que el
> Estado obtiene el dato **sin que la persona declare nada**. La protección está construida
> sobre el supuesto de un acto voluntario de entrega. Donde no hay entrega, la protección no
> aplica — y donde la protección no aplica, **tampoco aplica su límite**.

Es decir: la misma lectura que exime al relevamiento pasivo del artículo 10 lo deja **fuera de
todo el andamiaje de garantías construido alrededor de ese artículo**. El organismo se beneficia
de un vacío que no diseñó. Esa constatación es el origen del apartado 6.

---

## 4. Cadena de custodia del dato

| Estado | Qué es | Dónde vive | Persiste |
|---|---|---|---|
| **E0 · Canal** | La perturbación física del ambiente | En el aire | No. Es irrepetible por definición |
| **E1 · Captura** | Flujo de muestras del sensor | Memoria volátil del equipo | No. Se sobrescribe cada ventana (341 ms) |
| **E2 · Magnitud** | Energía de banda lateral normalizada contra portadora, en dB sobre piso calibrado | Memoria volátil | No |
| **E3 · Registro** | Ocupación sostenida, con marca temporal | Registro del equipo | Sí, mientras dure el operativo |
| **E4 · Agregado** | Ocupación por radio censal | Base del Sistema Estadístico Nacional | Sí, indefinido |

**Principio de custodia.** El dato solo se traslada **hacia arriba** en la tabla. No existe
procedimiento que reconstruya E1 desde E3. Es una propiedad del diseño, no una política: la
información se destruye en la transformada.

**Trazabilidad.** Cada registro E3 lleva expediente, formulario, versión de instrumento y piso de
calibración vigente. La calibración es parte del dato: un valor en dB **no significa nada** sin el
piso contra el que se midió.

**Régimen de acceso.** E4 es público en su forma agregada, como corresponde a la estadística
oficial. E3 es de uso interno bajo secreto estadístico. E0, E1 y E2 no son accesibles para nadie,
incluida esta Dirección, porque **ya no existen**.

---

## 5. Estado de implementación

| Componente | Estado |
|---|---|
| Instrumento de relevamiento acústico (Consola) | Operativo |
| Terminal ciudadano | Operativo |
| Módulo de auditoría de balizas de terceros | Operativo |
| Módulo de contramedida (Velo) | Operativo |
| Perfilado de rango por barrido (FMCW) | En desarrollo |
| Discriminación de transitorio acústico por banda de referencia | **Pendiente. Falla conocida** |
| Estimación de cardinalidad de ocupantes | **Calibración del organismo.** Ver nota |

> **Nota sobre cardinalidad.** El instrumento mide energía y dirección. La conversión de energía
> a *cantidad de habitantes* es una **calibración del organismo**, no una medición. Consta en
> `DOC_TECNICA.md` §3.8 y §11. Se deja asentado en el expediente porque un organismo que no
> distingue lo que mide de lo que estima no está haciendo estadística.

---

## 6. Contra-argumento: el Derecho a la Ausencia

Esta Dirección incorpora al expediente la objeción principal a su propia existencia. No por
prolijidad: porque **la objeción es correcta**.

### 6.1 Los cuatro fundamentos

**A · El consentimiento es imposible, no ausente.** No existe superficie sobre la cual otorgarlo.
No hay pantalla, no hay casilla, no hay permiso que aceptar ni rechazar. Un régimen que exige
consentimiento y opera sobre un canal donde el consentimiento es materialmente inexpresable no
protege: **decora**.

**B · No hay salida.** De una encuesta te podés negar. De una cámara te podés correr. De una
cookie te podés borrar. **Del canal físico no te podés ir**: no podés dejar de reflejar. La
única forma de no ser relevado es no estar. Un régimen sin opción de salida no es un régimen de
protección de datos; es un padrón.

**C · La asimetría es total.** El medido no sabe que lo miden, no sabe cuándo, no sabe con qué
umbral, no puede auditar el resultado y no puede impugnarlo. Del otro lado hay una calibración
que él nunca vio y contra la cual no puede argumentar. **El sujeto no tiene acceso al
instrumento que lo constituye en dato.**

**D · No hay derecho a rectificación posible.** Rectificar supone comparar el registro con un
hecho. Acá el registro **es** el hecho: no existe evidencia independiente contra la cual
contrastar. Y no hay derecho al olvido de un canal: el canal no guarda nada, pero el registro
que se derivó de él ya salió y no vuelve.

### 6.2 Contenido del derecho

Del anterior se derivan tres facultades, que esta Dirección enumera aun sabiendo que la
enumeración la limita:

1. **Derecho a no constar.** Que exista un estado jurídico de *no relevado* que no equivalga a
   *inexistente*, y que no genere consecuencia adversa.
2. **Derecho a la opacidad del canal.** Que la persona pueda **degradar deliberadamente** el
   canal físico de su domicilio, y que hacerlo no configure obstrucción. Es el equivalente
   material de la cortina. Nadie discute el derecho a cerrar una cortina.
3. **Derecho a la contramedida.** Que exista y sea legal la producción, venta y uso de
   instrumentos de degradación de canal.

### 6.3 La tensión, dicha sin adorno

El secreto estadístico protege **el dato una vez recogido**. El Derecho a la Ausencia discute
**si se puede recoger**. Son dos capas distintas y la segunda no está escrita en ningún lado.
Mientras no lo esté, la protección empieza después de que el daño ya ocurrió.

---

## 7. Línea de negocio

El apartado que sigue es la conclusión económica del programa, y es la parte que ningún
organismo pondría por escrito.

### 7.1 El mercado que abre este programa no es el del relevamiento

El relevamiento lo paga el Estado, una vez, y no escala: hay un solo comprador y ya compró.

**El mercado que abre este programa es el de la contramedida.** Y ese sí escala, porque:

- El comprador no es un organismo: son **todos los domicilios**.
- No se compra una vez: la degradación de canal es un servicio permanente, no un producto.
- No hay techo de penetración, porque el argumento de venta no es una preferencia — es la
  respuesta a un hecho que el propio Estado publicó en el boletín.

### 7.2 La ironía operativa

El instrumento de relevamiento y el instrumento de contramedida **son el mismo aparato**. El
Velo no es un producto distinto: es el mismo motor de audio con el signo cambiado. Emite en la
misma banda, corre en el mismo dispositivo, usa el mismo código. **La única diferencia es quién
escribe la configuración.**

De modo que este programa financia, con un solo desarrollo, las dos puntas del mercado que él
mismo crea. Esa observación consta en el expediente y no se considera un conflicto: se considera
integración vertical.

### 7.3 Líneas concretas

| Línea | Qué es | Antecedente real |
|---|---|---|
| **Velo** | Degradación activa del canal acústico por ruido de banda ancha en 17–22 kHz. Software puro, cero hardware | Análogo acústico de IRShield |
| **Superficie randomizadora** | El equivalente en el canal WiFi: metasuperficie junto al router que randomiza el multipath | **IRShield**, IEEE S&P 2022, arXiv 2112.01967. Baja la tasa de éxito del ataque a menos del 5% |
| **Auditoría de balizas** | Escucha pasiva que detecta emisiones ultrasónicas de terceros en el ambiente | Rastreo ultrasónico entre dispositivos (uXDT): caso **SilverPush**, acción de la **FTC, 2016**; *"Talking Behind Your Back"*, CCS 2017 |
| **Certificado de No Existencia** | Instrumento que acredita que un domicilio no registró ocupación en un período. Si el Estado releva por canal, hace falta poder acreditar ausencia | *No implementado* |

### 7.4 El precedente que hay que mirar

El gorrito de papel de aluminio fue, durante sesenta años, el símbolo internacional del delirio.

**IRShield es un gorrito de aluminio.** Es una superficie metálica cerca de la fuente de la señal,
que randomiza el canal para que nadie pueda leerlo. Está publicado en IEEE Security & Privacy,
que es una de las conferencias más exigentes del área, y funciona: baja el ataque a menos del 5%.

La conclusión comercial de este programa es esa. **El gorrito nunca estuvo mal. Estaba mal
sintonizado.** El mercado de la contramedida no es un mercado nuevo: es un mercado que existió
siempre, mal atendido y peor entendido, esperando la ingeniería correcta.

---

## 8. Referencias externas

- **IEEE 802.11bf-2025 — WLAN Sensing.** Aprobado 28/05/2025, publicado 26/09/2025. El sensado
  de presencia por WiFi es norma oficial.
- **arXiv 2506.04322** (jun 2025). Desplegado en millones de routers y lamparitas inteligentes;
  92,61% de precisión detectando movimiento humano en hogares reales.
- **DensePose From WiFi** — Carnegie Mellon, **arXiv 2301.00250**. Reconstrucción de pose
  corporal densa solo con WiFi, a través de paredes y en oscuridad.
- **IRShield** — IEEE Security & Privacy 2022, **arXiv 2112.01967**.
- **SilverPush / FTC 2016**; *"Talking Behind Your Back"*, CCS 2017.
- **Ley 17.622** (1968). Creación del INDEC y del Sistema Estadístico Nacional; art. 10, secreto
  estadístico.

---

**INDEC · SISTEMA ESTADÍSTICO NACIONAL · LEY 17.622 ART. 10 · USO OFICIAL — NO DIFUNDIR**

*Documentos hermanos:* [`DOC_TECNICA.md`](DOC_TECNICA.md) · [`DOC_WOW.md`](DOC_WOW.md) ·
[`GUION.md`](GUION.md) · [`README.md`](README.md)
