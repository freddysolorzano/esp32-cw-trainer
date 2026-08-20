/*
 * ============================================================
 *  ESP32 CW TRAINER & MORSE DECODER — YV4AA
 * ============================================================
 *  Morse code trainer and real-time decoder for the
 *  Radio Club Venezolano - Casa Regional Maracay (YV4AA).
 *
 *  Entrenador de código Morse y decodificador en tiempo real
 *  para el Radio Club Venezolano - Casa Regional Maracay (YV4AA).
 *
 * ------------------------------------------------------------
 *  HARDWARE / PINES
 * ------------------------------------------------------------
 *   GPIO 25  -> Audio buzzer / PWM output
 *   GPIO 27  -> Status LED
 *   GPIO 18  -> Straight key input (INPUT_PULLUP)
 *
 * ------------------------------------------------------------
 *  FEATURES / CARACTERÍSTICAS
 * ------------------------------------------------------------
 *  [EN] Morse player (text -> audio, 5-45 WPM), adaptive
 *       real-time decoder, captive portal WiFi, embedded web UI,
 *       NVS persistence, timing-consistency coaching metric.
 *  [ES] Reproductor Morse (texto -> audio, 5-45 WPM), decoder
 *       adaptativo en tiempo real, portal cautivo WiFi, interfaz
 *       web embebida, persistencia NVS, métrica de consistencia
 *       de timing para entrenamiento.
 *
 * ------------------------------------------------------------
 *  DECODER DESIGN / DISEÑO DEL DECODER
 * ------------------------------------------------------------
 *  [EN] Straight key only. Speed is estimated with the MEDIAN
 *       of recent dits (immune to outliers, adapts in 2-3
 *       elements). Classification uses a geometric midpoint
 *       with hysteresis (dit < 1.6x, dah > 2.4x, tolerant gray
 *       zone). Char/word ends follow real Morse structure
 *       (gap ~1 dit, char ~2.5, word ~5.5). In-progress chars
 *       are retro-corrected when speed changes >15%. Ambiguous
 *       elements never pollute the median; speed drops require
 *       4+ sustained slow elements; floor of ~8 WPM.
 *  [ES] Solo llave simple. La velocidad se estima con la MEDIANA
 *       de los dits recientes (inmune a outliers, se adapta en
 *       2-3 elementos). Clasificación por punto medio geométrico
 *       con histéresis (dit < 1.6x, dah > 2.4x, zona gris
 *       tolerante). Fines de carácter/palabra según estructura
 *       morse real (gap ~1 dit, char ~2.5, palabra ~5.5).
 *       Retro-corrección del carácter en curso si la velocidad
 *       cambia >15%. Los ambiguos nunca contaminan la mediana;
 *       bajar velocidad requiere 4+ lentos sostenidos; piso ~8 WPM.
 *
 *  Author / Autor:  Radio Club Venezolano YV4AA community project
 *  License / Licencia: MIT
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "index_html.h"
#include "logo.h"

// ==========================================
// CONFIGURACIÓN DE PINES
// ==========================================
#define AUDIO_PIN   25  // Buzzer o salida PWM
#define LED_PIN     27  // LED indicador
#define KEY_DOT     18  // Entrada Dit / Llave recta
#define KEY_DASH    19  // Entrada Dash

// ==========================================
// PARÁMETROS DE AUDIO / MORSE
// ==========================================
#define PWM_CHANNEL  0
#define PWM_RES      8

int currentWpm  = 15;
int ditDuration = 1200 / 15;
int currentTone = 700;
int currentVol  = 80;

volatile bool isPlaying = false;
volatile bool abortPlayback = false;
bool toneIsCurrentlyOn = false;

// ==========================================
// MOTOR POWER-DECODER (ESTRUCTURAS Y VARIABLES)
// ==========================================
#define DECODED_MAX_LEN  2000     // Máx. chars en buffer decodificado
#define KEY_STUCK_MS     2500UL   // Timeout llave atascada (2.5 s)
#define ELEMENT_MIN_MS   16UL     // Elemento mínimo real (16 ms)
#define ELEM_HIST_LEN    12       // Historial de elementos (mediana robusta)

String decodedBuffer = "";
int liveCalculatedWpm = 0;
int liveTimingPct = 100;          // 0-100: regularidad del timing (100 = perfecto)

// Estado del decoder (muestreo por polling, llave simple en GPIO 18)
int           treeIndex      = 1;   // posición actual en el árbol Morse
unsigned long pressStartMs   = 0;   // ms del flanco de bajada
unsigned long lastReleaseMs  = 0;   // ms del último flanco de subida
bool          keyIsActive    = false;
bool          decoderEnabled = true;   // decoder pauses when nobody is watching the DECODER tab / el decoder se pausa si nadie mira la pestaña DECODER
bool          charPending    = false;
bool          wordPending    = false;
int           charsInWord    = 0;       // chars desde el último espacio

// Historial de elementos para mediana robusta (inmune a outliers)
struct ElemRec { uint32_t dur; bool isDah; bool clean; uint32_t gapAfter; };
ElemRec elemHist[ELEM_HIST_LEN];
int elemHistHead  = 0;
int elemHistCount = 0;

// Carácter en construcción (para retro-corrección al cambiar la velocidad)
uint8_t curCode[8];   // 0=dit, 1=dah
int     curCodeLen = 0;

// Conteo de elementos lentos sostenidos (evidencia para bajar la velocidad)
int slowStreak = 0;

// Velocidad por MEDIANA (ms), inicializada a 15 WPM
float medianDitMs = 80.0f;    // mediana de dits recientes
float medianDahMs = 240.0f;   // mediana de dahs recientes
float medianGapMs = 80.0f;    // mediana de gaps inter-elemento (≈1 dit)

// Árbol binario Morse (128 nodos: soporta códigos de hasta 6 elementos)
// Índice 1 = Raíz. Si Dit -> 2*i. Si Dah -> 2*i + 1
const char MORSE_TREE[] = {
  '\0', // 0 no usado
  '\0', // 1 Raíz
  'E', 'T', // 2, 3
  'I', 'A', 'N', 'M', // 4..7
  'S', 'U', 'R', 'W', 'D', 'K', 'G', 'O', // 8..15
  'H', 'V', 'F', '\0', 'L', '\0', 'P', 'J', 'B', 'X', 'C', 'Y', 'Z', 'Q', '\0', '\0', // 16..31
  '5', '4', '\0', '3', '\0', '\0', '\0', '2', '\0', '\0', '+', '\0', '\0', '\0', '\0', '1', // 32..47
  '6', '=', '/', '\0', '\0', '\0', '(', '\0', '7', '\0', '\0', '\0', '8', '\0', '9', '0', // 48..63
  // 64..79 (76 = '?' ..--..; 69 = prosign SK, ver PROSIGNS)
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '?', '\0', '\0', '\0',
  // 80..95 (85 = '.' .-.-.-; 90 = '@' .--.-.)
  '\0', '\0', '\0', '\0', '\0', '.', '\0', '\0', '\0', '\0', '@', '\0', '\0', '\0', '\0', '\0',
  // 96..111 (107 = '!' -.-.--)
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '!', '\0', '\0', '\0', '\0',
  // 112..127 (115 = ',' --..--)
  '\0', '\0', '\0', ',', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

// Prosigns de salida múltiple (el árbol devuelve 1 char; estos expanden a texto)
struct Prosign { int idx; const char* out; };
const Prosign PROSIGNS[] = {
  {69, "SK"}   // ...-.- fin de contacto
};

// ==========================================
// CONFIGURACIÓN WI-FI & DNS (PORTAL CAUTIVO)
// ==========================================
const char* ap_ssid_default = "CW_Trainer_YV4AA";
const char* ap_pass_default = "12345678";
const byte DNS_PORT = 53;

IPAddress ap_ip(192, 168, 4, 1);
IPAddress ap_gw(192, 168, 4, 1);
IPAddress ap_mask(255, 255, 255, 0);

WebServer server(80);
DNSServer dnsServer;
Preferences prefs;

String sta_ssid = "";
String sta_password = "";

// Tabla Morse para reproducción
struct MorseMap {
  char c;
  const char* code;
};

const MorseMap morseTable[] = {
  {'A', ".-"},    {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
  {'E', "."},     {'F', "..-."},  {'G', "--."},   {'H', "...."},
  {'I', ".."},    {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
  {'M', "--"},    {'N', "-."},    {'O', "---"},   {'P', ".--."},
  {'Q', "--.-"},  {'R', ".-."},   {'S', "..."},   {'T', "-"},
  {'U', "..-"},   {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
  {'Y', "-.--"},  {'Z', "--.."},  {'1', ".----"}, {'2', "..---"},
  {'3', "...--"}, {'4', "....-"}, {'5', "....."}, {'6', "-...."},
  {'7', "--..."}, {'8', "---.."}, {'9', "----."}, {'0', "-----"},
  {'/', "-..-."}, {'?', "..--.."},{'.', ".-.-.-"},{',', "--..--"},
  {'=', "-...-"}, {'!', "-.-.--"},{'@', ".--.-."}
};

// ==========================================
// CONTROL DE AUDIO Y LED
// ==========================================
void soundAndLightOn() {
  if (!toneIsCurrentlyOn) {
    if (currentVol > 0) {
      int duty = map(currentVol, 0, 100, 0, 128);
      #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWriteTone(AUDIO_PIN, currentTone);
        ledcWrite(AUDIO_PIN, duty);
      #else
        ledcWriteTone(PWM_CHANNEL, currentTone);
        ledcWrite(PWM_CHANNEL, duty);
      #endif
    }
    digitalWrite(LED_PIN, HIGH);
    toneIsCurrentlyOn = true;
  }
}

void soundAndLightOff() {
  if (toneIsCurrentlyOn) {
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
      ledcWrite(AUDIO_PIN, 0);
    #else
      ledcWrite(PWM_CHANNEL, 0);
    #endif
    digitalWrite(LED_PIN, LOW);
    toneIsCurrentlyOn = false;
  }
}

bool waitAbortable(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    dnsServer.processNextRequest();
    server.handleClient();
    if (abortPlayback) {
      soundAndLightOff();
      return false;
    }
    delay(1);
  }
  return true;
}

bool sendDit() {
  soundAndLightOn();
  if (!waitAbortable(ditDuration)) return false;
  soundAndLightOff();
  return waitAbortable(ditDuration);
}

bool sendDah() {
  soundAndLightOn();
  if (!waitAbortable(ditDuration * 3)) return false;
  soundAndLightOff();
  return waitAbortable(ditDuration);
}

bool playMorseLetter(const char* morse) {
  for (int i = 0; morse[i] != '\0'; i++) {
    if (abortPlayback) return false;
    if (morse[i] == '.') {
      if (!sendDit()) return false;
    } else if (morse[i] == '-') {
      if (!sendDah()) return false;
    }
  }
  return waitAbortable(ditDuration * 2);
}

void playTextMorse(String text) {
  isPlaying = true;
  abortPlayback = false;
  text.toUpperCase();
  if (text == "SK") {
    playMorseLetter("...-.-");
  } else {
    for (int i = 0; i < text.length(); i++) {
      if (abortPlayback) break;
      char ch = text[i];
      if (ch == ' ') {
        if (!waitAbortable(ditDuration * 4)) break;
        continue;
      }
      for (const auto& item : morseTable) {
        if (item.c == ch) {
          if (!playMorseLetter(item.code)) break;
          break;
        }
      }
    }
  }
  soundAndLightOff();
  isPlaying = false;
  abortPlayback = false;
}

// ==========================================
// MOTOR POWER-DECODER (llave simple + mediana + estructura morse)
// ==========================================

void appendDecoded(char c) {
  if (decodedBuffer.length() >= DECODED_MAX_LEN) {
    decodedBuffer.remove(0, 500); // descartar lo más viejo
  }
  decodedBuffer += c;
}

// ---- Utilidades de mediana (robustas a outliers) ----
void addElemHist(uint32_t dur, bool isDah, bool clean, uint32_t gapAfter) {
  elemHist[elemHistHead] = {dur, isDah, clean, gapAfter};
  elemHistHead = (elemHistHead + 1) % ELEM_HIST_LEN;
  if (elemHistCount < ELEM_HIST_LEN) elemHistCount++;
}

float medianOf(float* arr, int n) {
  // Ordenamiento por inserción (n <= 12, suficiente)
  for (int i = 1; i < n; i++) {
    float key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) { arr[j+1] = arr[j]; j--; }
    arr[j+1] = key;
  }
  return (n % 2) ? arr[n/2] : (arr[n/2 - 1] + arr[n/2]) / 2.0f;
}

// Recalcular medianas desde el historial (SOLO elementos limpios + rechazo de outliers)
void recomputeMedians() {
  float dits[ELEM_HIST_LEN], dahs[ELEM_HIST_LEN];
  int nd = 0, nh = 0;
  for (int i = 0; i < elemHistCount; i++) {
    int idx = (elemHistHead - 1 - i + ELEM_HIST_LEN) % ELEM_HIST_LEN;
    // v11.1: solo elementos limpios (no ambiguos) entran a la mediana
    // Only clean (non-ambiguous) elements feed the median
    if (!elemHist[idx].clean) continue;
    // Rechazo de outliers: un dit no puede ser >1.8x la mediana actual
    // (evita que una duda/pausa puntual infle la mediana y baje el WPM)
    if (!elemHist[idx].isDah && elemHist[idx].dur > medianDitMs * 1.8f) continue;
    if (elemHist[idx].isDah && elemHist[idx].dur > medianDahMs * 1.8f) continue;
    if (elemHist[idx].isDah) dahs[nh++] = (float)elemHist[idx].dur;
    else dits[nd++] = (float)elemHist[idx].dur;
  }
  if (nd >= 2) medianDitMs = medianOf(dits, nd);
  if (nh >= 2) medianDahMs = medianOf(dahs, nh);
  // Consistencia: Dah >= 2.5x Dit
  if (medianDahMs < medianDitMs * 2.5f) medianDahMs = medianDitMs * 3.0f;
  if (medianDitMs > medianDahMs / 2.2f) medianDitMs = medianDahMs / 3.0f;
  // Piso de velocidad: nunca bajar de ~8 WPM (dit > 150ms) salvo evidencia real
  if (medianDitMs > 150.0f) medianDitMs = 150.0f;
}

// Actualizar la métrica de regularidad del timing (0-100)
void updateTimingPct() {
  if (elemHistCount < 3) { liveTimingPct = 100; return; }
  float vals[ELEM_HIST_LEN];
  int n = 0;
  for (int i = 0; i < elemHistCount; i++) {
    int idx = (elemHistHead - 1 - i + ELEM_HIST_LEN) % ELEM_HIST_LEN;
    float v = elemHist[idx].isDah ? (float)elemHist[idx].dur / 3.0f : (float)elemHist[idx].dur;
    vals[n++] = v;
  }
  float mean = 0;
  for (int i = 0; i < n; i++) mean += vals[i];
  mean /= n;
  if (mean < 1.0f) { liveTimingPct = 100; return; }
  float var = 0;
  for (int i = 0; i < n; i++) { float d = vals[i] - mean; var += d * d; }
  var /= n;
  float cv = sqrt(var) / mean;  // coeficiente de variación
  // CV <= 0.10 -> 100% ; CV >= 0.45 -> 0%
  liveTimingPct = constrain((int)(100.0f - (cv - 0.10f) * 285.0f), 0, 100);
}

// Clasificar elemento por punto medio geométrico con histéresis
// dit si dur < 1.6x dit ; dah si dur > 2.4x dit ; zona gris -> por cercanía a 2.0x
bool classifyElem(float durMs, bool* ambiguous) {
  float ditMax = medianDitMs * 1.6f;  // claramente dit
  float dahMin = medianDitMs * 2.4f;  // claramente dah
  if (durMs < ditMax) { *ambiguous = false; return false; }  // dit
  if (durMs > dahMin) { *ambiguous = false; return true; }   // dah
  *ambiguous = true;  // zona gris: tolerar, clasificar por cercanía al punto medio
  return durMs >= medianDitMs * 2.0f;
}

// Retro-corrección: reclasificar el carácter en curso si la velocidad cambió
void retroCorrectChar() {
  if (curCodeLen == 0 || elemHistCount == 0) return;
  int startIdx = (elemHistHead - curCodeLen + ELEM_HIST_LEN) % ELEM_HIST_LEN;
  treeIndex = 1;
  for (int i = 0; i < curCodeLen; i++) {
    int idx = (startIdx + i) % ELEM_HIST_LEN;
    bool amb;
    bool isDah = classifyElem((float)elemHist[idx].dur, &amb);
    elemHist[idx].isDah = isDah;
    curCode[i] = isDah ? 1 : 0;
    if (treeIndex < 128) treeIndex = treeIndex * 2 + (isDah ? 1 : 0);
  }
}

void processRobustDecoder() {
  if (isPlaying) return;

  bool currentPinState = (digitalRead(KEY_DOT) == LOW);  // llave simple GPIO 18
  unsigned long now = millis();

  // Decoder paused -> ONLY key feedback (sound/LED), no decoding.
  // Decoder pausado -> SOLO feedback de llave (sonido/LED), sin decodificar.
  // The key ALWAYS beeps when tapped (trainer); only recognition stops.
  // La llave SIEMPRE suena al teclear (entrenador); solo se detiene el reconocimiento.
  if (!decoderEnabled) {
    if (keyIsActive && (now - pressStartMs) >= KEY_STUCK_MS) {
      soundAndLightOff();
      keyIsActive = false;
      lastReleaseMs = now;
    }
    if (currentPinState && !keyIsActive) {
      soundAndLightOn();
      pressStartMs = now;
      keyIsActive = true;
    } else if (!currentPinState && keyIsActive) {
      soundAndLightOff();
      lastReleaseMs = now;
      keyIsActive = false;
    }
    return;
  }

  // Timeout de llave atascada: si queda pulsada >2.5 s, resetear estado
  if (keyIsActive && (now - pressStartMs) >= KEY_STUCK_MS) {
    soundAndLightOff();
    keyIsActive = false;
    lastReleaseMs = now;
    treeIndex = 1;
    charPending = false;
    wordPending = false;
    curCodeLen = 0;
  }

  // Flanco de bajada (Pulsación)
  if (currentPinState && !keyIsActive) {
    soundAndLightOn();
    pressStartMs = now;
    keyIsActive = true;
    charPending = true;
    wordPending = true;
  }
  // Flanco de subida (Liberación)
  else if (!currentPinState && keyIsActive) {
    soundAndLightOff();
    unsigned long duration = now - pressStartMs;
    unsigned long gapBefore = (lastReleaseMs > 0) ? (pressStartMs - lastReleaseMs) : 0;
    lastReleaseMs = now;
    keyIsActive = false;

    // Filtro Antirrebote (>16 ms)
    if (duration < ELEMENT_MIN_MS) return;

    float durMs = (float)duration;
    bool ambiguous;
    bool isDah = classifyElem(durMs, &ambiguous);
    bool clean = !ambiguous;

    // Asymmetric adaptation: speed up fast, slow down only with
    // sustained evidence (4+ consecutive slow elements). A single
    // hesitation no longer mis-calibrates the decoder.
    // Adaptación asimétrica: subir velocidad rápido, bajarla solo con
    // evidencia sostenida (4+ elementos lentos consecutivos).
    // Una duda/pausa puntual ya NO descalibra el decoder.
    if (clean && !isDah) {
      if (durMs > medianDitMs * 1.25f) slowStreak++;
      else slowStreak = 0;
    } else {
      slowStreak = 0;
    }
    if (slowStreak < 4 && durMs > medianDitMs * 1.8f) {
      // Elemento lento sin evidencia sostenida: NO toca la mediana (outlier)
      clean = false;
    }

    // Guardar en historial + carácter en curso
    addElemHist(duration, isDah, clean, gapBefore);
    if (curCodeLen < 8) curCode[curCodeLen++] = isDah ? 1 : 0;
    if (treeIndex < 128) treeIndex = treeIndex * 2 + (isDah ? 1 : 0);

    // Recalcular medianas con el historial
    float prevDit = medianDitMs;
    recomputeMedians();

    // Si la velocidad estimada cambió >15%, retro-corregir el carácter en curso
    if (curCodeLen > 1 && fabs(medianDitMs - prevDit) / prevDit > 0.15f) {
      retroCorrectChar();
    }

    // WPM por mediana (estable)
    liveCalculatedWpm = constrain((int)(1200.0f / medianDitMs), 5, 50);
    updateTimingPct();
  }
  // Estado Libre: decodificación de letras y espacios
  else if (!keyIsActive) {
    float silenceMs = (float)(now - lastReleaseMs);

    // Umbrales por estructura morse: gap elemento ≈ 1 dit, char ≈ 3, palabra ≈ 7
    float charEndMs  = max(medianDitMs * 2.5f, medianDitMs * 1.8f);  // ~2.5 dits
    float wordEndMs  = max(medianDitMs * 5.5f, medianDitMs * 4.0f);  // ~5.5 dits

    // Fin de carácter
    if (charPending && silenceMs >= charEndMs) {
      if (treeIndex > 1 && treeIndex < 128) {
        // Prosigns multi-char (ej: SK)
        const char* prosign = nullptr;
        for (const auto& p : PROSIGNS) {
          if (p.idx == treeIndex) { prosign = p.out; break; }
        }
        if (prosign) {
          for (const char* q = prosign; *q != '\0'; q++) appendDecoded(*q);
        } else {
          char c = MORSE_TREE[treeIndex];
          if (c != '\0') {
            appendDecoded(c);
          } else {
            appendDecoded('?');
          }
        }
        charsInWord++;
      }
      treeIndex = 1; // Reiniciar árbol
      charPending = false;
      curCodeLen = 0;
    }

    // Fin de palabra
    if (wordPending && silenceMs >= wordEndMs) {
      if (charsInWord > 0) {
        appendDecoded(' ');
        charsInWord = 0;
      }
      wordPending = false;
    }
  }
}

// ==========================================
// PERSISTENCIA DE CONFIGURACIÓN (NVS)
// ==========================================
void saveSettings() {
  prefs.begin("cw_config", false);
  prefs.putInt("wpm", currentWpm);
  prefs.putInt("tone", currentTone);
  prefs.putInt("vol", currentVol);
  prefs.end();
}

// ==========================================
// RUTAS DEL SERVIDOR WEB
// ==========================================
void handleRoot() { 
  server.send(200, "text/html", PAGE_HTML);
}

void handleSetDecoder() {
  if (server.hasArg("state")) {
    bool newState = (server.arg("state") == "1");
    if (newState != decoderEnabled) {
      decoderEnabled = newState;
      // Al pausar/reanudar, resetear estado del decoder para no arrastrar
      // elementos a medio decodificar.
      treeIndex = 1;
      charPending = false;
      wordPending = false;
      charsInWord = 0;
      curCodeLen = 0;
      keyIsActive = false;
      soundAndLightOff();
      lastReleaseMs = millis();
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleSetWPM() {
  if (server.hasArg("val")) {
    currentWpm = constrain(server.arg("val").toInt(), 5, 45);
    ditDuration = 1200 / currentWpm;
    // The playback slider does NOT re-anchor the decoder anymore.
    // El slider de reproducción YA NO re-ancla el decoder.
    // The decoder is self-adaptive: starts at 15 WPM and calibrates
    // itself with real keying (EMA + re-anchor). Moving the slider
    // only changes the audio playback speed.
    // El decoder es auto-adaptativo: arranca en 15 WPM y se ajusta solo
    // con el tecleo real. Mover el slider solo cambia la velocidad de
    // reproducción de audio.
    saveSettings();
  }
  server.send(200, "text/plain", "OK");
}

void handleSetFreq() {
  if (server.hasArg("val")) {
    currentTone = constrain(server.arg("val").toInt(), 300, 1500);
    if (toneIsCurrentlyOn) {
      #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWriteTone(AUDIO_PIN, currentTone);
      #else
        ledcWriteTone(PWM_CHANNEL, currentTone);
      #endif
    }
    saveSettings();
  }
  server.send(200, "text/plain", "OK");
}

void handleSetVol() {
  if (server.hasArg("val")) {
    currentVol = constrain(server.arg("val").toInt(), 0, 100);
    saveSettings();
  }
  server.send(200, "text/plain", "OK");
}

void handlePlay() {
  if (server.hasArg("text")) {
    String text = server.arg("text");
    // Respond AFTER playback finishes so the frontend (auto-play)
    // applies the PAUSA AUTO slider at the right moment.
    // Responder DESPUÉS de reproducir. Así el frontend (auto-play)
    // aplica la pausa del slider PAUSA AUTO en el momento correcto.
    playTextMorse(text);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Falta texto");
  }
}

void handleStop() {
  abortPlayback = true;
  soundAndLightOff();
  server.send(200, "text/plain", "STOPPED");
}

void handleGetStatus() {
  String statusHtml = "";
  if (WiFi.status() == WL_CONNECTED) {
    statusHtml = "🟢 Conectado a: <b>" + WiFi.SSID() + "</b> (IP: " + WiFi.localIP().toString() + ")";
  } else {
    statusHtml = "🔵 Modo AP Activo (SSID: <b>" + String(ap_ssid_default) + "</b> | IP: 192.168.4.1)";
  }
  server.send(200, "application/json", "{\"wifi_status\":\"" + statusHtml + "\"}");
}

void handleScanWiFi() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleSaveConfig() {
  prefs.begin("cw_config", false);
  if (server.hasArg("ssid")) {
    sta_ssid = server.arg("ssid");
    prefs.putString("ssid", sta_ssid);
  }
  if (server.hasArg("pass")) {
    sta_password = server.arg("pass");
    prefs.putString("pass", sta_password);
  }
  prefs.end();
  server.send(200, "text/plain", "OK");
  if (sta_ssid.length() > 0) {
    WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  }
}

void handleClearWiFi() {
  prefs.begin("cw_config", false);
  prefs.remove("ssid");
  prefs.remove("pass");
  prefs.end();
  sta_ssid = "";
  sta_password = "";
  WiFi.disconnect(true);
  server.send(200, "text/plain", "CLEARED");
}

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);

  prefs.begin("cw_config", true);
  sta_ssid     = prefs.getString("ssid", "");
  sta_password = prefs.getString("pass", "");
  currentWpm   = prefs.getInt("wpm", 15);
  currentTone  = prefs.getInt("tone", 700);
  currentVol   = prefs.getInt("vol", 80);
  prefs.end();

  currentWpm  = constrain(currentWpm, 5, 45);
  currentTone = constrain(currentTone, 300, 1500);
  currentVol  = constrain(currentVol, 0, 100);
  ditDuration = 1200 / currentWpm;
  // The decoder ALWAYS starts at 15 WPM, independent of the stored
  // NVS WPM (which is playback-only). Self-adaptive from real keying.
  // El decoder SIEMPRE arranca en 15 WPM, independiente del
  // WPM guardado en NVS (que es solo de reproducción).
  // Median-based initial speed (dit=80ms, dah=240ms, gap=80ms)
  // Medianas iniciales a 15 WPM (dit=80ms, dah=240ms, gap=80ms)
  medianDitMs = 80.0f;
  medianDahMs = 240.0f;
  medianGapMs = 80.0f;
  elemHistCount = 0;
  elemHistHead = 0;
  curCodeLen = 0;
  slowStreak = 0;
  liveTimingPct = 100;

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  #if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(AUDIO_PIN, currentTone, PWM_RES);
    ledcWrite(AUDIO_PIN, 0);
  #else
    ledcSetup(PWM_CHANNEL, currentTone, PWM_RES);
    ledcAttachPin(AUDIO_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);
  #endif

  pinMode(KEY_DOT, INPUT_PULLUP);
  pinMode(KEY_DASH, INPUT_PULLUP);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(ap_ip, ap_gw, ap_mask);
  WiFi.softAP(ap_ssid_default, ap_pass_default);

  dnsServer.start(DNS_PORT, "*", ap_ip);

  if (sta_ssid.length() > 0) {
    WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  }

  server.on("/", handleRoot);

  server.on("/logo.svg", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Cache-Control", "public, max-age=86400");
    server.send_P(200, "image/svg+xml; charset=utf-8", LOGO_SVG, strlen_P(LOGO_SVG));
  });

  server.on("/set_wpm", handleSetWPM);
  server.on("/set_decoder", handleSetDecoder);
  server.on("/set_freq", handleSetFreq);
  server.on("/set_vol", handleSetVol);
  server.on("/play", handlePlay);
  server.on("/stop", handleStop);
  server.on("/get_status", handleGetStatus);
  server.on("/scan_wifi", handleScanWiFi);
  server.on("/save_cfg", handleSaveConfig);
  server.on("/clear_wifi", handleClearWiFi);

  server.on("/get_decoded", HTTP_GET, []() {
    String jsonText = decodedBuffer;
    jsonText.replace("\\", "\\\\");
    jsonText.replace("\"", "\\\"");
    jsonText.replace("\n", "\\n");
    jsonText.replace("\r", "\\r");
    jsonText.replace("\t", "\\t");
    String json = "{\"text\":\"" + jsonText + "\",\"wpm\":" + String(liveCalculatedWpm) +
                  ",\"timing\":" + String(liveTimingPct) + "}";
    server.send(200, "application/json", json);
  });

  server.on("/clear_decoded", HTTP_GET, []() {
    decodedBuffer = "";
    treeIndex = 1;
    charPending = false;
    wordPending = false;
    charsInWord = 0;
    curCodeLen = 0;
    elemHistCount = 0;
    elemHistHead = 0;
    slowStreak = 0;
    medianDitMs = 80.0f;
    medianDahMs = 240.0f;
    medianGapMs = 80.0f;
    liveCalculatedWpm = 0;
    liveTimingPct = 100;
    server.send(200, "text/plain", "OK");
  });

  // Portal Cautivo
  server.on("/generate_204", handleRoot);
  server.on("/gen_204", handleRoot);
  server.on("/hotspot-detect.html", handleRoot);
  server.on("/canonical.html", handleRoot);
  server.on("/ncsi.txt", handleRoot);
  server.on("/connecttest.txt", handleRoot);

  server.onNotFound([]() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });

  server.begin();
  Serial.println("\n[OK] CW Trainer & Robust Power-Decoder Activo (v10)");
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  processRobustDecoder();
  delay(1);
}
