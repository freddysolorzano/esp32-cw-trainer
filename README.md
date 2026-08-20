# ESP32 CW Trainer & Morse Decoder — YV4AA

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/)
[![Arduino Core](https://img.shields.io/badge/Arduino%20Core-2.0.6-green.svg)](https://github.com/espressif/arduino-esp32)
[![Status: Beta](https://img.shields.io/badge/Status-Beta-orange.svg)](#)

> ⚠️ **BETA / EN FASE BETA** — This project is under active development. The firmware is functional and tested, but APIs, thresholds and features may change. **Electronic component specifications (BOM, schematic, PCB) will be published soon.** / Este proyecto está en desarrollo activo. El firmware es funcional y probado, pero las APIs, umbrales y funciones pueden cambiar. **Próximamente se publicarán las especificaciones de componentes electrónicos (BOM, esquemático, PCB).**

---

**Morse code trainer + real-time adaptive decoder** for the Radio Club Venezolano — Casa Regional Maracay (YV4AA).

**Entrenador de código Morse + decodificador adaptativo en tiempo real** para el Radio Club Venezolano — Casa Regional Maracay (YV4AA).

---

### 📸 Web UI / Interfaz web

<img src="screenshots/web-ui.png" alt="CW Trainer Web UI" width="320">

## 🇬🇧 English

### What is it?

A standalone **ESP32** device that:

- **Plays** Morse code from text (buzzer + LED), 5–45 WPM, tone 300–1500 Hz
- **Decodes** a straight key in real time with an adaptive, robust decoder
- Runs a **captive portal WiFi** with an embedded dark-theme web UI (no app needed, works on any phone)
- **Coaches timing**: shows a live consistency score (0–100%) so operators can improve their rhythm without frustration
- Persists settings (WPM / tone / volume / WiFi) in NVS flash

Built for radio clubs, classrooms, and self-training.

### Hardware / Pinout

| GPIO | Function |
|------|----------|
| 25 | Audio buzzer / PWM output |
| 27 | Status LED |
| 18 | Straight key input (INPUT_PULLUP) |

### Features

- **Morse player**: text → audio with LED; prosign **SK**; adjustable WPM/tone/volume
- **Adaptive decoder** (straight key only):
  - Speed estimated with the **median** of recent dits — immune to outliers, adapts in 2–3 elements
  - Classification with **geometric midpoint + hysteresis** (dit < 1.6×, dah > 2.4×, tolerant gray zone)
  - Char/word ends follow the **real Morse structure** (element gap ≈ 1 dit, char ≈ 2.5, word ≈ 5.5)
  - **Retro-correction** of the in-progress character when speed changes > 15%
  - Ambiguous elements **never pollute** the median; slowing down requires 4+ sustained slow elements; ~8 WPM floor
- **Timing coach**: live consistency metric (coefficient of variation → 0–100%), color-coded in the UI
- **Captive portal** WiFi: own AP `CW_Trainer_YV4AA` (pass `12345678`), plus STA mode with saved credentials
- **Web UI**: 5 tabs (List, Text, Keyboard, Decoder, Config), dark theme
  - List tab: flashcards, **shuffle mode**, infinite auto-play, blind mode (hidden word), auto-pause slider
  - Decoder tab: live decoded text, WPM, **timing badge**, Morse legend
- **NVS persistence** for WPM/tone/volume and WiFi credentials

### Getting started

1. Install [Arduino IDE](https://www.arduino.cc/en/software) with the [ESP32 core 2.0.6](https://github.com/espressif/arduino-esp32)
2. Open `cw_trainer_v9.ino` (keep `index_html.h` and `logo.h` in the same folder)
3. Select your ESP32 board (e.g. "ESP32 Dev Module")
4. Wire the buzzer to GPIO 25, LED to GPIO 27, straight key to GPIO 18 (and GND)
5. Flash and power on
6. Connect to the `CW_Trainer_YV4AA` WiFi (password `12345678`) and open `http://192.168.4.1`

### How the decoder works (short version)

Morse timing has a strict structure: dit = 1 unit, dah = 3, element gap = 1, char gap = 3, word gap = 7. The decoder:

1. Measures every key press duration (polling, 16 ms debounce)
2. Estimates the dit duration with a **median** of recent dits (robust)
3. Classifies each element against the **geometric midpoint** with hysteresis
4. Emits characters/words when silences match the Morse structure
5. **Retro-corrects** the current character if the speed estimate shifts
6. Computes a **consistency score** (CV of normalized durations) for coaching

### Tunable parameters

| Constant | Location | Default | Meaning |
|----------|----------|---------|---------|
| `ELEMENT_MIN_MS` | `.ino` | 16 ms | Debounce / minimum element |
| Hysteresis dit/dah | `classifyElem()` | 1.6× / 2.4× | Classification thresholds |
| Char end | decoder | ~2.5 dits | Silence to end a character |
| Word end | decoder | ~5.5 dits | Silence to end a word |
| Speed-change retro | decoder | > 15% | Retro-correct trigger |
| Slow-evidence | decoder | 4 elements | Required to lower speed |
| Speed floor | decoder | ~8 WPM | Minimum estimated speed |

### License

[MIT](LICENSE) — free to use, modify, and share. Built with ❤️ for the amateur radio community.

---

## 🇻🇪 Español

### ¿Qué es?

Un dispositivo **ESP32** autónomo que:

- **Reproduce** código Morse desde texto (buzzer + LED), 5–45 WPM, tono 300–1500 Hz
- **Decodifica** una llave simple en tiempo real con un decoder adaptativo y robusto
- Levanta un **portal cautivo WiFi** con interfaz web embebida en tema oscuro (sin app, funciona en cualquier teléfono)
- **Entrena el timing**: muestra un puntaje de consistencia en vivo (0–100%) para que los operadores mejoren su ritmo sin frustración
- Persiste la configuración (WPM / tono / volumen / WiFi) en flash NVS

Hecho para radio clubes, aulas y auto-entrenamiento.

### Hardware / Pines

| GPIO | Función |
|------|---------|
| 25 | Buzzer de audio / salida PWM |
| 27 | LED indicador |
| 18 | Entrada de llave simple (INPUT_PULLUP) |

### Características

- **Reproductor Morse**: texto → audio con LED; prosign **SK**; WPM/tono/volumen ajustables
- **Decoder adaptativo** (solo llave simple):
  - Velocidad estimada con la **mediana** de los dits recientes — inmune a outliers, se adapta en 2–3 elementos
  - Clasificación con **punto medio geométrico + histéresis** (dit < 1.6×, dah > 2.4×, zona gris tolerante)
  - Fines de carácter/palabra según la **estructura morse real** (gap entre elementos ≈ 1 dit, carácter ≈ 2.5, palabra ≈ 5.5)
  - **Retro-corrección** del carácter en curso cuando la velocidad cambia > 15%
  - Los elementos ambiguos **nunca contaminan** la mediana; bajar la velocidad requiere 4+ elementos lentos sostenidos; piso de ~8 WPM
- **Entrenador de timing**: métrica de consistencia en vivo (coeficiente de variación → 0–100%), con colores en la UI
- **Portal cautivo** WiFi: AP propio `CW_Trainer_YV4AA` (clave `12345678`), más modo STA con credenciales guardadas
- **Interfaz web**: 5 pestañas (Lista, Texto, Teclado, Decoder, Config), tema oscuro
  - Pestaña Lista: flashcards, **modo aleatorio**, auto-reproducción infinita, modo oculto (palabra escondida), slider de pausa automática
  - Pestaña Decoder: texto decodificado en vivo, WPM, **badge de timing**, leyenda Morse
- **Persistencia NVS** para WPM/tono/volumen y credenciales WiFi

### Primeros pasos

1. Instala [Arduino IDE](https://www.arduino.cc/en/software) con el [core ESP32 2.0.6](https://github.com/espressif/arduino-esp32)
2. Abre `cw_trainer_v9.ino` (mantén `index_html.h` y `logo.h` en la misma carpeta)
3. Selecciona tu placa ESP32 (ej. "ESP32 Dev Module")
4. Conecta el buzzer al GPIO 25, el LED al GPIO 27, la llave simple al GPIO 18 (y GND)
5. Flashea y enciende
6. Conéctate a la WiFi `CW_Trainer_YV4AA` (clave `12345678`) y abre `http://192.168.4.1`

### Cómo funciona el decoder (versión corta)

El timing Morse tiene una estructura estricta: dit = 1 unidad, dah = 3, gap entre elementos = 1, gap entre caracteres = 3, gap entre palabras = 7. El decoder:

1. Mide cada duración de pulsación (polling, antirrebote de 16 ms)
2. Estima la duración del dit con una **mediana** de los dits recientes (robusta)
3. Clasifica cada elemento contra el **punto medio geométrico** con histéresis
4. Emite caracteres/palabras cuando los silencios coinciden con la estructura Morse
5. **Retro-corrige** el carácter actual si la estimación de velocidad cambia
6. Calcula un **puntaje de consistencia** (CV de duraciones normalizadas) para el entrenamiento

### Parámetros ajustables

| Constante | Ubicación | Default | Significado |
|-----------|-----------|---------|-------------|
| `ELEMENT_MIN_MS` | `.ino` | 16 ms | Antirrebote / elemento mínimo |
| Histéresis dit/dah | `classifyElem()` | 1.6× / 2.4× | Umbrales de clasificación |
| Fin de carácter | decoder | ~2.5 dits | Silencio para cerrar un carácter |
| Fin de palabra | decoder | ~5.5 dits | Silencio para cerrar una palabra |
| Retro por cambio de velocidad | decoder | > 15% | Disparo de retro-corrección |
| Evidencia para bajar | decoder | 4 elementos | Requeridos para bajar la velocidad |
| Piso de velocidad | decoder | ~8 WPM | Velocidad mínima estimada |

### Licencia

[MIT](LICENSE) — libre de usar, modificar y compartir. Hecho con ❤️ para la comunidad de radioaficionados.

---

## 🤝 Contributing / Contribuciones

Found a bug or want a feature? Open an issue or a PR. Ideas welcome: paddle support, OTA updates, more prosigns, session logs, iambic mode.

¿Encontraste un bug o quieres una mejora? Abre un issue o un PR. Ideas bienvenidas: soporte de paddle, actualización OTA, más prosigns, registro de sesiones, modo iámbico.

**73 de YV4AA** 📻
