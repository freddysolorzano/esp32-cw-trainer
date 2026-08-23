# CW Decoder — Technical Documentation / Documentación Técnica

**Decoder architecture as implemented in the current firmware (v12).**
**Arquitectura del decoder tal como está implementada en el firmware actual (v12).**

> ⚠️ **BETA** — Thresholds and behavior may change during active development.
> ⚠️ **BETA** — Los umbrales y el comportamiento pueden cambiar durante el desarrollo activo.

---

## 🇬🇧 English

### 1. Design philosophy

The decoder uses **deferred per-character decision**:

- While the operator is keying, the firmware only **measures raw durations** and buffers them **without classifying** them.
- When the end-of-character silence arrives, **all elements of the character are classified at once** against a **stable median**.

This eliminates the most damaging class of bugs of earlier designs: classifying element-by-element against a median that was being recalculated mid-character, plus retro-correction rewriting history → cascading contamination → drifting WPM and growing error rates over time.

### 2. Five layers

```
1. EDGE DETECTOR (polling)      digitalRead(GPIO32) + millis(), debounce >16 ms
                               → events: PRESS(t) / RELEASE(t)
2. CHARACTER ASSEMBLER          accumulates raw durations charBuf[] (unclassified)
                               + measures intra-character gaps (release→press)
3. SPEED ESTIMATOR (unit = dit) median of "1-dit" samples (dits + gaps) with
                               double median, hard outlier, ±25% rate-limit
4. DEFERRED CLASSIFIER          classifies full charBuf → Morse tree → character
                               + char/word end detection by silence
5. TRAINING METRIC (🎯)         CV (coefficient of variation) over dits + gaps → 0-100
```

### 3. Data structures

| Variable | Size | Purpose |
|---|---|---|
| `charBuf[8]` | 8 | Raw durations of the character being keyed (Morse ≤ 6 elements + noise margin) |
| `charDits[6]` | 6 | Speeds of the last **complete** characters (FIFO ring; median of this is the stable speed) |
| `ditSamples[12]` | 12 | "1-dit" samples for speed: classified dits **+ intra-character gaps** |
| `ditHist[12]` | 12 | History for the 🎯 timing metric (dits + gaps) |
| `medianDitMs` | — | Current stable speed in ms/dit. Starts at **80 ms = 15 WPM** |
| `charPending` / `wordPending` | — | State flags: character/word in progress |
| `keyWasStuck` | — | Latch that blocks re-triggering until a stuck key is truly released |

### 4. Step-by-step flow (`processRobustDecoder()`)

Runs on every `loop()` iteration (pure polling, **no ISR** — the ISR version caused key-response regression).

**Falling edge (key pressed):**
- If a character is in progress, measure the **intra-character gap** (silence since last release). If between 16 ms and `2.1 × medianDitMs`, store it as a speed sample **and** feed the 🎯 metric.
  - *Why this is the key idea:* the gap between elements of the same character always measures ~1 dit of the real operator, **immune to classification**. Even if the median is anchored fast (e.g. 15 WPM) and a slow dit gets misclassified as a dah, the gap still feeds the median → **no deadlock when the operator slows down**.
- Turn sound + LED on, set `charPending = true`.

**Rising edge (key released):**
- Turn sound off, compute `duration = now - pressStartMs`.
- **Debounce:** if `duration < 16 ms` → discard (mechanical key bounce).
- Otherwise store the **raw** duration in `charBuf[]`. Nothing else. No classification here.

**Silence → structure ends** (computed from the stable median, real Morse structure — not polluted averages):

- **Character end:** `silence ≥ 2.2 × medianDitMs` → trigger `classifyCharToTree()`.
  - 2.2× because the inter-character gap is ~3 dits (with human jitter it can drop to ~2.6×) while the intra-character gap is ~1 dit. The former 2.5× merged characters under human jitter.
- **Word end:** `silence ≥ 6.0 × medianDitMs` → insert a space (only if `charsInWord > 0`, avoiding ghost spaces).

### 5. dit/dah classification

Against the **stable median** (never mid-character):

```
dur < 1.5 × median  → dit
dur > 2.0 × median  → dah
gray zone [1.5, 2.0] → decide by proximity to the midpoint 1.75×
```

- 1.75× does not punish short human dahs (real ~2.2× instead of the theoretical 3×), which was the failure of the 1.6×/2.4× thresholds.
- The character is decoded by walking a **128-node binary Morse tree**: `dit → 2*i`, `dah → 2*i+1`. O(log n), no lookup tables. Supports codes up to 6 elements (`@` index 90, `!` 107, `?` 76, `.` 85, `,` 115).
- Multi-char prosigns: the tree returns 1 char; `PROSIGNS[]` expands index 69 → `"SK"` (end of contact).

### 6. Speed estimation (`updateSpeedFromChar`)

Runs **only when a valid character closes** (never mid-word):

1. **Samples:** classified dits of the character + intra-character gaps → `ditSamples[12]`.
2. **Double median:** median of the 12 samples → character speed → pushed into `charDits[6]` → `medianDitMs = median(charDits)`.
3. **Hard outlier:** if the new median is outside `0.4×–2.5×` of the current one → discarded (an absurd sample cannot recalibrate).
4. **±25% per-character rate-limit:** the new speed is clamped to `[0.75×, 1.25×]` of the current one. This makes the WPM display movement deterministic (no more "WPM rising by itself").

**Slow re-anchoring (with restore):**
If a character comes out invalid (`?`), the operator is probably **slower** than the median:

```c
medianDitMs = max(medianDitMs * 1.8f, 60.0f);   // increase dit = lower speed
```

- Reclassify with the ×1.8 median; if now valid → accept (the new samples are consolidated by `updateSpeedFromChar()` with the re-anchored median).
- If still invalid → **restore** the previous value.
- Note: it is `×1.8` (not `÷1.8`): dividing would make slow dits even more likely to be classified as dahs, worsening the deadlock.
- The re-anchor operates **outside** the ±25% clamp (deliberate jump); the clamp only limits normal drift.

### 7. Training metric 🎯 (`updateTimingPct`)

Measures **rhythm regularity** (CV = coefficient of variation) over dits **and intra-character gaps**:

```
CV = stddev / mean
CV ≤ 0.10 → 100%   (near-perfect rhythm)
CV ≥ 0.45 → 0%
linear mapping: 100 - (cv - 0.10) × 285
```

Real human jitter (±12–15% ⇒ CV ≈ 0.12–0.15) scores ~85–90, which is honest feedback for a trainer.

### 8. Robustness / error handling

- **Stuck key:** if held >2.5 s → sound off and **latch `keyWasStuck`** until the key is truly released (prevents the old infinite 2.5 s-on / 1 ms-off retrigger loop).
- **Invalid character (noise):** emits `?` and does **not** update speed or 🎯 (garbage never contaminates the windows).
- **Bounded buffer:** 800 chars, trims by 200. Web polling is **incremental** (`/get_decoded?since=N`) → HTTP never freezes key sampling.
- **Loop order:** key is sampled **before** `server.handleClient()` → less edge jitter.
- **`medianOf()` does not mutate the ring** (local copy) → FIFO insertion order preserved.

### 9. Tunable parameters

| Constant | Location | Default | Meaning |
|---|---|---|---|
| `ELEMENT_MIN_MS` | `.ino` | 16 ms | Debounce / minimum element |
| `KEY_STUCK_MS` | `.ino` | 2500 ms | Stuck-key timeout |
| `medianDitMs` init | `.ino` | 80 ms (15 WPM) | Neutral start; converges in 2-3 chars |
| `CHAR_BUF_MAX` | `.ino` | 8 | Max elements per char (Morse ≤ 6 + margin) |
| `CHAR_DITS_MAX` | `.ino` | 6 | Completed-char speeds (median) |
| `DIT_HIST_MAX` | `.ino` | 12 | 🎯 metric history |
| `DIT_SAMP_MAX` | `.ino` | 12 | Speed samples (dits + gaps) |
| dit threshold | `isDahElem()` | < 1.5× | Classify as dit |
| dah threshold | `isDahElem()` | > 2.0× | Classify as dah |
| gray zone | `isDahElem()` | 1.75× | Midpoint proximity decision |
| char end | decoder | 2.2× | Silence to close a character |
| word end | decoder | 6.0× | Silence to close a word |
| gap as sample (cap) | decoder | 2.1× | Max gap accepted as speed sample |
| slow re-anchor | decoder | ×1.8 (floor 60 ms) | Invalid char → retry with slower median |
| hard outlier | `updateSpeedFromChar()` | 0.4×–2.5× | Absurd samples discarded |
| rate-limit | `updateSpeedFromChar()` | ±25% | Max speed change per character |
| 🎯 CV mapping | `updateTimingPct()` | 0.10→100%, 0.45→0% | CV to score mapping |
| `DECODED_MAX_LEN` | `.ino` | 800 | Decoded buffer length |

---

## 🇻🇪 Español

### 1. Filosofía de diseño

El decoder usa **decisión diferida por carácter**:

- Mientras el operador teclea, el firmware solo **mide duraciones crudas** y las acumula **sin clasificarlas**.
- Cuando llega el silencio de fin de carácter, **todos los elementos del carácter se clasifican de una vez** contra una **mediana estable**.

Esto elimina la clase de bugs más dañina de diseños anteriores: clasificar elemento a elemento contra una mediana que se recalculaba a mitad de carácter, más retro-corrección que reescribía el historial → contaminación en cascada → WPM a la deriva y errores crecientes con el tiempo.

### 2. Cinco capas

```
1. DETECTOR DE FLANCOS (polling)  digitalRead(GPIO32) + millis(), antirrebote >16 ms
                                 → eventos: PRESS(t) / RELEASE(t)
2. ENSAMBLADOR DE CARÁCTER        acumula duraciones crudas charBuf[] (sin clasificar)
                                 + mide gaps intra-carácter (release→press)
3. ESTIMADOR DE VELOCIDAD (unidad = dit) mediana de muestras "1-dit" (dits + gaps)
                                 con doble mediana, outlier duro, rate-limit ±25%
4. CLASIFICADOR DIFERIDO          clasifica charBuf completo → árbol Morse → carácter
                                 + detección de fin de carácter/palabra por silencio
5. MÉTRICA DE ENTRENAMIENTO (🎯)  CV (coeficiente de variación) sobre dits + gaps → 0-100
```

### 3. Estructuras de datos

| Variable | Tamaño | Función |
|---|---|---|
| `charBuf[8]` | 8 | Duraciones crudas del carácter en construcción (Morse ≤ 6 elementos + margen anti-ruido) |
| `charDits[6]` | 6 | Velocidades de los últimos caracteres **completos** (anillo FIFO; su mediana es la velocidad estable) |
| `ditSamples[12]` | 12 | Muestras "1-dit" para velocidad: dits clasificados **+ gaps intra-carácter** |
| `ditHist[12]` | 12 | Historial para la métrica 🎯 (dits + gaps) |
| `medianDitMs` | — | Velocidad estable actual en ms/dit. Arranca en **80 ms = 15 WPM** |
| `charPending` / `wordPending` | — | Flags de estado: carácter/palabra en construcción |
| `keyWasStuck` | — | Latch que bloquea el re-disparo hasta que una llave atascada se libere de verdad |

### 4. Flujo paso a paso (`processRobustDecoder()`)

Se ejecuta en cada vuelta del `loop()` (polling puro, **sin ISR** — la versión con ISR causó regresión de respuesta de la llave).

**Flanco de bajada (pulsas la llave):**
- Si hay carácter en curso, mide el **gap intra-carácter** (silencio desde la última liberación). Si está entre 16 ms y `2.1 × medianDitMs`, lo guarda como muestra de velocidad **y** alimenta la métrica 🎯.
  - *Por qué es la pieza clave:* el gap entre elementos del mismo carácter siempre mide ~1 dit del operador real, **inmune a la clasificación**. Aunque la mediana esté anclada rápida (ej. 15 WPM) y un dit lento se mal-clasifique como dah, el gap sigue alimentando la mediana → **no hay deadlock cuando el operador desacelera**.
- Enciende sonido + LED, marca `charPending = true`.

**Flanco de subida (sueltas la llave):**
- Apaga el sonido, calcula `duration = now - pressStartMs`.
- **Antirrebote:** si `duration < 16 ms` → descarta (rebote de llave mecánica).
- Si no: guarda la duración **cruda** en `charBuf[]`. Nada más. Aquí no se clasifica.

**Silencio → fines de estructura** (calculados sobre la mediana estable, estructura morse real — no promedios contaminados):

- **Fin de carácter:** `silencio ≥ 2.2 × medianDitMs` → dispara `classifyCharToTree()`.
  - 2.2× porque el gap entre caracteres es ~3 dits (con jitter humano puede caer a ~2.6×) mientras el intra-carácter es ~1 dit. El 2.5× anterior fusionaba caracteres con jitter humano.
- **Fin de palabra:** `silencio ≥ 6.0 × medianDitMs` → inserta un espacio (solo si `charsInWord > 0`, evita espacios fantasma).

### 5. Clasificación dit/dah

Contra la **mediana estable** (nunca a mitad de carácter):

```
dur < 1.5 × mediana  → dit
dur > 2.0 × mediana  → dah
zona gris [1.5, 2.0] → decisión por cercanía al punto medio 1.75×
```

- El 1.75× no castiga los dahs humanos cortos (reales ~2.2× en vez de los 3× teóricos), que era el fallo de los umbrales 1.6×/2.4×.
- El carácter se decodifica recorriendo un **árbol binario Morse de 128 nodos**: `dit → 2*i`, `dah → 2*i+1`. O(log n), sin tablas de búsqueda. Soporta códigos de hasta 6 elementos (`@` índice 90, `!` 107, `?` 76, `.` 85, `,` 115).
- Prosigns multi-carácter: el árbol devuelve 1 char; `PROSIGNS[]` expande el índice 69 → `"SK"` (fin de contacto).

### 6. Estimación de velocidad (`updateSpeedFromChar`)

Corre **solo cuando se cierra un carácter válido** (nunca a mitad de palabra):

1. **Muestras:** dits clasificados del carácter + gaps intra-carácter → `ditSamples[12]`.
2. **Doble mediana:** mediana de las 12 muestras → velocidad del carácter → se empuja a `charDits[6]` → `medianDitMs = mediana(charDits)`.
3. **Outlier duro:** si la mediana nueva queda fuera de `0.4×–2.5×` de la actual → se descarta (una muestra absurda no puede descalibrar).
4. **Rate-limit ±25% por carácter:** la velocidad nueva se clampea a `[0.75×, 1.25×]` de la actual. Hace determinista el movimiento del WPM mostrado (fin del "WPM que sube solo").

**Re-anclaje lento (con restauración):**
Si un carácter sale inválido (`?`), el operador probablemente va **más lento** que la mediana:

```c
medianDitMs = max(medianDitMs * 1.8f, 60.0f);   // aumentar dit = bajar velocidad
```

- Reclasificar con la mediana ×1.8; si ahora sale válido → aceptar (las muestras nuevas las consolida `updateSpeedFromChar()` con la mediana re-anclada).
- Si sigue inválido → **restaurar** el valor previo.
- Nota: es `×1.8` (no `÷1.8`): dividir haría que los dits lentos se clasifiquen aún más como dahs, agravando el deadlock.
- El re-anclaje opera **fuera** del clamp ±25% (salto deliberado); el clamp solo limita el drift normal.

### 7. Métrica de entrenamiento 🎯 (`updateTimingPct`)

Mide la **regularidad del ritmo** (CV = coeficiente de variación) sobre dits **y gaps intra-carácter**:

```
CV = desviación estándar / media
CV ≤ 0.10 → 100%   (ritmo casi perfecto)
CV ≥ 0.45 → 0%
mapeo lineal: 100 - (cv - 0.10) × 285
```

El jitter humano real (±12–15% ⇒ CV ≈ 0.12–0.15) puntúa ~85–90, un feedback honesto para un entrenador.

### 8. Robustez / manejo de errores

- **Llave atascada:** si queda pulsada >2.5 s → apaga el sonido y **activa el latch `keyWasStuck`** hasta que la llave se libere de verdad (evita el antiguo bucle infinito 2.5 s on / 1 ms off).
- **Carácter inválido (ruido):** emite `?` y **no** actualiza velocidad ni 🎯 (la basura nunca contamina las ventanas).
- **Buffer acotado:** 800 chars, recorta de a 200. El polling web es **incremental** (`/get_decoded?since=N`) → el HTTP nunca congela el muestreo de la llave.
- **Orden del loop:** la llave se muestrea **antes** de `server.handleClient()` → menos jitter de flancos.
- **`medianOf()` no muta el anillo** (copia local) → se preserva el orden de inserción FIFO.

### 9. Parámetros ajustables

| Constante | Ubicación | Default | Significado |
|---|---|---|---|
| `ELEMENT_MIN_MS` | `.ino` | 16 ms | Antirrebote / elemento mínimo |
| `KEY_STUCK_MS` | `.ino` | 2500 ms | Timeout de llave atascada |
| `medianDitMs` inicial | `.ino` | 80 ms (15 WPM) | Arranque neutro; converge en 2-3 chars |
| `CHAR_BUF_MAX` | `.ino` | 8 | Máx. elementos por carácter (Morse ≤ 6 + margen) |
| `CHAR_DITS_MAX` | `.ino` | 6 | Velocidades de caracteres completos (mediana) |
| `DIT_HIST_MAX` | `.ino` | 12 | Historial de la métrica 🎯 |
| `DIT_SAMP_MAX` | `.ino` | 12 | Muestras de velocidad (dits + gaps) |
| Umbral dit | `isDahElem()` | < 1.5× | Clasifica como dit |
| Umbral dah | `isDahElem()` | > 2.0× | Clasifica como dah |
| Zona gris | `isDahElem()` | 1.75× | Decisión por cercanía al punto medio |
| Fin de carácter | decoder | 2.2× | Silencio para cerrar un carácter |
| Fin de palabra | decoder | 6.0× | Silencio para cerrar una palabra |
| Gap como muestra (techo) | decoder | 2.1× | Gap máximo aceptado como muestra de velocidad |
| Re-anclaje lento | decoder | ×1.8 (piso 60 ms) | Carácter inválido → reintentar con mediana más lenta |
| Outlier duro | `updateSpeedFromChar()` | 0.4×–2.5× | Muestras absurdas descartadas |
| Rate-limit | `updateSpeedFromChar()` | ±25% | Cambio máximo de velocidad por carácter |
| Mapeo CV 🎯 | `updateTimingPct()` | 0.10→100%, 0.45→0% | Mapeo de CV a puntaje |
| `DECODED_MAX_LEN` | `.ino` | 800 | Largo del buffer decodificado |
