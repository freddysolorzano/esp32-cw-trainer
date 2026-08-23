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
 *   GPIO 32  -> Straight key input (INPUT_PULLUP)
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
 *  [EN] v12: deferred per-character decision. Raw element
 *       durations are buffered WITHOUT classifying them; when
 *       the char-end silence arrives, ALL elements of the
 *       character are classified at once against a STABLE
 *       median (dit < 1.5x, dah > 2.0x, gray zone by proximity
 *       to 1.75x). Speed is updated ONLY from completed chars
 *       (median of the last 6 char speeds, clamped to +-25% per
 *       char) so the displayed WPM never jumps by itself.
 *       No retro-correction, no history pollution, no forced
 *       dah/dit consistency that could collapse the median.
 *       Char/word ends follow real Morse structure (char ~2.5,
 *       word ~5.5). /get_decoded?since=N is incremental so HTTP
 *       polling never blocks key sampling.
 *  [ES] v12: decisión diferida por carácter. Las duraciones
 *       crudas se acumulan SIN clasificar; al llegar el silencio
 *       de fin de carácter, TODOS los elementos se clasifican de
 *       una vez contra una mediana ESTABLE (dit < 1.5x, dah >
 *       2.0x, zona gris por cercanía a 1.75x). La velocidad se
 *       actualiza SOLO con caracteres completos (mediana de las
 *       últimas 6 velocidades, clamp +-25% por carácter) — el
 *       WPM mostrado ya no salta solo. Sin retro-corrección, sin
 *       contaminación del historial, sin consistencia forzada
 *       dah/dit que pudiera colapsar la mediana. Fines de
 *       carácter/palabra por estructura morse real (char ~2.5,
 *       palabra ~5.5). /get_decoded?since=N es incremental para
 *       que el polling HTTP nunca bloquee el muestreo de llave.
 *  v12.2: rate-limit ±25% real por carácter; gaps intra-carácter
 *       en la métrica 🎯; gate de velocidad/🎯 con caracteres
 *       válidos; medianOf() sin mutar el anillo; loop muestrea
 *       llave antes que HTTP; doc re-anclaje ×1.8 corregida.
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
#define KEY_DOT     32  // Entrada Dit / Llave recta (única entrada en v11)

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
// MOTOR POWER-DECODER v12 (ESTRUCTURAS Y VARIABLES)
// ==========================================
#define DECODED_MAX_LEN  800      // Máx. chars en buffer decodificado (v12: liviano)
#define KEY_STUCK_MS     2500UL   // Timeout llave atascada (2.5 s)
#define ELEMENT_MIN_MS   16UL     // Elemento mínimo real (16 ms)
#define CHAR_BUF_MAX     8        // Máx. elementos por carácter (morse ≤ 6 + margen)
#define CHAR_DITS_MAX    6        // Velocidades de caracteres completos (mediana)
#define DIT_HIST_MAX     12       // Dits recientes (métrica de regularidad 🎯)
#define DIT_SAMP_MAX     12       // Muestras de dit (dits + gaps intra-carácter) para velocidad

String decodedBuffer = "";
uint32_t decodedTotal = 0;        // total de chars decodificados (para ?since=N)
int liveCalculatedWpm = 0;
int liveTimingPct = 100;          // 0-100: regularidad del timing (100 = perfecto)

// Estado del decoder (muestreo por polling, llave simple en GPIO 32)
unsigned long pressStartMs   = 0;   // ms del flanco de bajada
unsigned long lastReleaseMs  = 0;   // ms del último flanco de subida
bool          keyIsActive    = false;
bool          keyWasStuck    = false;   // llave atascada: bloquea re-disparo hasta liberarla (FIX 2026-08-22)
bool          decoderEnabled = true;   // decoder pauses when nobody is watching the DECODER tab / el decoder se pausa si nadie mira la pestaña DECODER
bool          charPending    = false;
bool          wordPending    = false;
int           charsInWord    = 0;       // chars desde el último espacio

// Carácter en construcción: duraciones CRUDAS (nada se clasifica aún)
float charBuf[CHAR_BUF_MAX];
int   charBufLen = 0;

// Velocidad: mediana de dits de los últimos caracteres COMPLETOS
float charDits[CHAR_DITS_MAX];
int   charDitsHead  = 0;
int   charDitsCount = 0;

// Dits recientes crudos (solo métrica de regularidad)
float ditHist[DIT_HIST_MAX];
int   ditHistHead  = 0;
int   ditHistCount = 0;

// Muestras de velocidad: dits clasificados + gaps intra-carácter (~1 dit).
// Los gaps son la clave para velocidad baja: un dit lento mal clasificado
// como dah nunca alimentaría la mediana (deadlock); el gap intra-carácter
// SIEMPRE mide ~1 dit y adapta la velocidad aunque la clasificación falle.
float ditSamples[DIT_SAMP_MAX];
int   ditSampHead  = 0;
int   ditSampCount = 0;

// Velocidad estable actual (ms), inicializada a 15 WPM
float medianDitMs = 80.0f;

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
// MOTOR POWER-DECODER v12 (decisión diferida por carácter)
// ==========================================

void appendDecoded(char c) {
  if (decodedBuffer.length() >= DECODED_MAX_LEN) {
    decodedBuffer.remove(0, 200); // descartar lo más viejo (de a 200)
  }
  decodedBuffer += c;
  decodedTotal++;
}

// ---- Utilidades de mediana (n <= 12, ordenamiento por inserción) ----
float medianOf(float* arr, int n) {
  // v12.2: copia local para NO mutar el arreglo original (el anillo FIFO
  // charDits/ditSamples debe conservar su orden de inserción).
  float tmp[16];
  for (int i = 0; i < n; i++) tmp[i] = arr[i];
  for (int i = 1; i < n; i++) {
    float key = tmp[i];
    int j = i - 1;
    while (j >= 0 && tmp[j] > key) { tmp[j+1] = tmp[j]; j--; }
    tmp[j+1] = key;
  }
  return (n % 2) ? tmp[n/2] : (tmp[n/2 - 1] + tmp[n/2]) / 2.0f;
}

// Clasificar UN elemento contra la mediana ESTABLE.
// dit < 1.5x ; dah > 2.0x ; zona gris -> cercanía al punto medio 1.75x
bool isDahElem(float durMs) {
  if (durMs < medianDitMs * 1.5f) return false;
  if (durMs > medianDitMs * 2.0f) return true;
  return durMs >= medianDitMs * 1.75f;
}

// Añadir una muestra de velocidad (dit clasificado o gap intra-carácter)
void addDitSample(float v) {
  if (v < ELEMENT_MIN_MS) return;
  ditSamples[ditSampHead] = v;
  ditSampHead = (ditSampHead + 1) % DIT_SAMP_MAX;
  if (ditSampCount < DIT_SAMP_MAX) ditSampCount++;
}

// Decisión DIFERIDA: clasificar el carácter completo y devolver su índice
// en el árbol morse (1 = raíz/inválido). Nunca a mitad de carácter.
int classifyCharToTree() {
  int tr = 1;
  for (int i = 0; i < charBufLen; i++) {
    bool isDah = isDahElem(charBuf[i]);
    if (tr < 128) tr = tr * 2 + (isDah ? 1 : 0);
  }
  return tr;
}

// Actualizar velocidad con las muestras de dit (dits clasificados + gaps).
// - Doble mediana (12 muestras + 6 velocidades): robusta y sin saltos.
// - Outlier duro amplio: una muestra absurda no descalibra.
void updateSpeedFromChar() {
  // Dits clasificados de este carácter (refuerzan cuando la clasificación acierta)
  for (int i = 0; i < charBufLen; i++) {
    float dur = charBuf[i];
    if (isDahElem(dur)) continue;
    ditHist[ditHistHead] = dur;
    ditHistHead = (ditHistHead + 1) % DIT_HIST_MAX;
    if (ditHistCount < DIT_HIST_MAX) ditHistCount++;
    addDitSample(dur);
  }
  if (ditSampCount < 3) return;   // pocas muestras: no tocar la velocidad
  float tmp[DIT_SAMP_MAX];
  for (int i = 0; i < ditSampCount; i++) {
    int idx = (ditSampHead - 1 - i + DIT_SAMP_MAX) % DIT_SAMP_MAX;
    tmp[i] = ditSamples[idx];
  }
  float nm = medianOf(tmp, ditSampCount);
  if (nm < medianDitMs * 0.4f || nm > medianDitMs * 2.5f) return;  // outlier duro
  // v12.2: RATE-LIMIT ±25% por carácter (implementa el clamp documentado).
  // Techo de movimiento por carácter: el WPM mostrado no puede saltar solo.
  // El re-anclaje lento (×1.8) queda FUERA de este clamp (salto deliberado).
  float lo = medianDitMs * 0.75f;
  float hi = medianDitMs * 1.25f;
  if (nm < lo) nm = lo;
  if (nm > hi) nm = hi;
  charDits[charDitsHead] = nm;
  charDitsHead = (charDitsHead + 1) % CHAR_DITS_MAX;
  if (charDitsCount < CHAR_DITS_MAX) charDitsCount++;
  medianDitMs = medianOf(charDits, charDitsCount);
}

// Actualizar la métrica de regularidad del timing (0-100)
void updateTimingPct() {
  if (ditHistCount < 3) { liveTimingPct = 100; return; }
  float vals[DIT_HIST_MAX];
  int n = 0;
  for (int i = 0; i < ditHistCount; i++) {
    int idx = (ditHistHead - 1 - i + DIT_HIST_MAX) % DIT_HIST_MAX;
    vals[n++] = ditHist[idx];
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

void processRobustDecoder() {
  if (isPlaying) return;

  bool currentPinState = (digitalRead(KEY_DOT) == LOW);  // llave simple GPIO 32
  unsigned long now = millis();

  // Decoder paused -> ONLY key feedback (sound/LED), no decoding.
  // Decoder pausado -> SOLO feedback de llave (sonido/LED), sin decodificar.
  // The key ALWAYS beeps when tapped (trainer); only recognition stops.
  // La llave SIEMPRE suena al teclear (entrenador); solo se detiene el reconocimiento.
  if (!decoderEnabled) {
    // Llave liberada: habilita la llave de nuevo tras un stuck
    if (!currentPinState) keyWasStuck = false;

    if (keyIsActive && (now - pressStartMs) >= KEY_STUCK_MS) {
      soundAndLightOff();
      keyIsActive = false;
      keyWasStuck = true;      // FIX: no re-disparar mientras siga pulsada
      lastReleaseMs = now;
    }
    if (currentPinState && !keyIsActive && !keyWasStuck) {
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

  // Llave liberada: habilita la llave de nuevo tras un stuck
  if (!currentPinState) keyWasStuck = false;

  // Timeout de llave atascada: si queda pulsada >2.5 s, apagar y BLOQUEAR
  // hasta que se libere (FIX 2026-08-22: antes re-disparaba el tono al instante)
  if (keyIsActive && (now - pressStartMs) >= KEY_STUCK_MS) {
    soundAndLightOff();
    keyIsActive = false;
    keyWasStuck = true;
    lastReleaseMs = now;
    charPending = false;
    wordPending = false;
    charBufLen = 0;
  }

  // Flanco de bajada (Pulsación)
  if (currentPinState && !keyIsActive && !keyWasStuck) {
    // v12.1: gap intra-carácter como muestra de velocidad (~1 dit exacto).
    // Es INMUNE a la clasificación dit/dah: aunque la mediana esté anclada
    // rápido (p.ej. 15 WPM) y un dit lento se mal-clasifique como dah, el
    // silencio entre elementos del mismo carácter SIEMPRE mide ~1 dit del
    // operador real -> la velocidad baja se aprende sin deadlock.
    // v12.2: el gap también alimenta la métrica 🎯 (timing constante =
    // dits Y espacios consistentes; antes solo se medían los dits).
    if (charPending && lastReleaseMs > 0 && charBufLen > 0) {
      float gapMs = (float)(now - lastReleaseMs);
      if (gapMs > ELEMENT_MIN_MS && gapMs < medianDitMs * 2.1f) {
        addDitSample(gapMs);
        ditHist[ditHistHead] = gapMs;
        ditHistHead = (ditHistHead + 1) % DIT_HIST_MAX;
        if (ditHistCount < DIT_HIST_MAX) ditHistCount++;
      }
    }
    soundAndLightOn();
    pressStartMs = now;
    keyIsActive = true;
    charPending = true;
    wordPending = true;
  }
  // Flanco de subida (Liberación): SOLO se guarda la duración CRUDA.
  // v12: nada se clasifica aquí; la decisión es diferida al fin de carácter.
  else if (!currentPinState && keyIsActive) {
    soundAndLightOff();
    unsigned long duration = now - pressStartMs;
    lastReleaseMs = now;
    keyIsActive = false;

    // Filtro Antirrebote (>16 ms)
    if (duration < ELEMENT_MIN_MS) return;

    if (charBufLen < CHAR_BUF_MAX) {
      charBuf[charBufLen++] = (float)duration;
    }
  }
  // Estado Libre: decodificación de letras y espacios (decisión diferida)
  else if (!keyIsActive) {
    float silenceMs = (float)(now - lastReleaseMs);

    // Umbrales por estructura morse real sobre la mediana ESTABLE.
    // charEnd 2.2x: por debajo del gap entre caracteres (3 dits, con jitter
    // humano puede caer a ~2.6x) pero por encima del gap entre elementos
    // (~1 dit). wordEnd 6.0x: gap entre palabras (~7 dits).
    float charEndMs = medianDitMs * 2.2f;
    float wordEndMs = medianDitMs * 6.0f;

    // Fin de carácter: clasificar TODO el carácter de una vez
    if (charPending && silenceMs >= charEndMs) {
      if (charBufLen > 0) {
        int tr = classifyCharToTree();
        bool valid = (tr > 1 && tr < 128 && MORSE_TREE[tr] != '\0');

        // v12.1 RE-ANCLAJE LENTO: si el carácter sale inválido, el operador
        // probablemente va MÁS LENTO que la mediana actual (dits largos mal
        // clasificados como dahs). Probar reclasificar con mediana ×1.8
        // (AUMENTAR el dit = bajar la velocidad estimada; dividir haría lo
        // contrario y agravaría el deadlock — NO cambiar a ÷1.8).
        if (!valid) {
          float prevMedian = medianDitMs;
          medianDitMs = max(medianDitMs * 1.8f, 60.0f);   // bajar velocidad estimada
          int tr2 = classifyCharToTree();
          bool valid2 = (tr2 > 1 && tr2 < 128 && MORSE_TREE[tr2] != '\0');
          if (valid2) {
            tr = tr2;
            valid = true;
            // v12.2: NO empujar directo a charDits aquí. La velocidad nueva
            // (más lenta) la consolida updateSpeedFromChar() con las muestras
            // de este carácter (que ahora clasifican bien con la mediana
            // re-anclada). Evita el doble push con los mismos elementos.
            // El re-anclaje queda FUERA del clamp ±25% (salto deliberado).
          } else {
            medianDitMs = prevMedian;  // no era eso: restaurar
          }
        }

        // v12.2: SOLO caracteres válidos actualizan velocidad y métrica 🎯.
        // Un carácter inválido (basura/ruido) no debe contaminar las ventanas.
        if (valid) {
          // Prosigns multi-char (ej: SK)
          const char* prosign = nullptr;
          for (const auto& p : PROSIGNS) {
            if (p.idx == tr) { prosign = p.out; break; }
          }
          if (prosign) {
            for (const char* q = prosign; *q != '\0'; q++) appendDecoded(*q);
          } else {
            appendDecoded(MORSE_TREE[tr]);
          }
          charsInWord++;
          // Actualizar velocidad con caracteres completos (mediana estable)
          updateSpeedFromChar();
          updateTimingPct();
          liveCalculatedWpm = constrain((int)(1200.0f / medianDitMs), 5, 50);
        } else {
          appendDecoded('?');
        }
      }
      charPending = false;
      charBufLen = 0;
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
      charPending = false;
      wordPending = false;
      charsInWord = 0;
      charBufLen = 0;
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
    // Límite de seguridad: texto máximo de reproducción (evita abusos)
    if (text.length() > 500) text = text.substring(0, 500);
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
    String ssid = WiFi.SSID();
    ssid.replace("&", "&amp;");
    ssid.replace("<", "&lt;");
    ssid.replace(">", "&gt;");
    ssid.replace("\"", "&quot;");
    statusHtml = "🟢 Conectado a: <b>" + ssid + "</b> (IP: " + WiFi.localIP().toString() + ")";
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
    String ssid = WiFi.SSID(i);
    ssid.replace("\\", "\\\\");
    ssid.replace("\"", "\\\"");
    json += "{\"ssid\":\"" + ssid + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleSaveConfig() {
  prefs.begin("cw_config", false);
  if (server.hasArg("ssid")) {
    sta_ssid = server.arg("ssid");
    if (sta_ssid.length() > 32) sta_ssid = sta_ssid.substring(0, 32);  // límite WPA
    prefs.putString("ssid", sta_ssid);
  }
  if (server.hasArg("pass")) {
    sta_password = server.arg("pass");
    if (sta_password.length() > 64) sta_password = sta_password.substring(0, 64);  // límite WPA2
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
  // Median-based initial speed (dit=80ms -> 15 WPM). Self-adaptive from
  // real keying: starts at 15 WPM and calibrates with completed chars.
  // Velocidad inicial por mediana a 15 WPM (dit=80ms); se auto-ajusta
  // con los caracteres completos del tecleo real.
  medianDitMs = 80.0f;
  charBufLen = 0;
  charDitsHead = 0;
  charDitsCount = 0;
  ditHistHead = 0;
  ditHistCount = 0;
  ditSampHead = 0;
  ditSampCount = 0;
  decodedTotal = 0;
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
    // v12: polling INCREMENTAL. Si el cliente manda ?since=N solo se
    // serializan los chars NUEVOS (O(1) en vez de O(buffer)). Esto evita
    // que el handler HTTP bloquee el loop y corrompa el muestreo de llave.
    // v12.2: aritmética consistente en uint32_t (sin cast a int).
    uint32_t since = server.hasArg("since") ? (uint32_t)server.arg("since").toInt() : 0;
    uint32_t bufLen = decodedBuffer.length();
    int64_t bufStart = (int64_t)decodedTotal - (int64_t)bufLen;  // puede ser negativo tras trim
    bool full = false;
    String out;
    if ((int64_t)since <= bufStart || bufLen == 0) {
      out = decodedBuffer;   // cliente muy atrás (o sin since): enviar todo
      full = true;
    } else {
      out = decodedBuffer.substring((uint32_t)((int64_t)since - bufStart));
    }
    out.replace("\\", "\\\\");
    out.replace("\"", "\\\"");
    out.replace("\n", "\\n");
    out.replace("\r", "\\r");
    out.replace("\t", "\\t");
    String json = "{\"n\":" + String(decodedTotal) + ",\"full\":" + (full ? "true" : "false") +
                  ",\"text\":\"" + out + "\",\"wpm\":" + String(liveCalculatedWpm) +
                  ",\"timing\":" + String(liveTimingPct) + "}";
    server.send(200, "application/json", json);
  });

  server.on("/clear_decoded", HTTP_GET, []() {
    decodedBuffer = "";
    decodedTotal = 0;
    charPending = false;
    wordPending = false;
    charsInWord = 0;
    charBufLen = 0;
    charDitsHead = 0;
    charDitsCount = 0;
    ditHistHead = 0;
    ditHistCount = 0;
    ditSampHead = 0;
    ditSampCount = 0;
    medianDitMs = 80.0f;
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
  Serial.println("\n[OK] CW Trainer & Robust Power-Decoder Activo (v12.2)");
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================
void loop() {
  // v12.2: muestrear la llave ANTES de atender HTTP. server.handleClient()
  // puede bloquear unos ms con clientes lentos; el decoder es el camino
  // sensible a latencia, así que va primero (menos jitter de flancos).
  processRobustDecoder();
  dnsServer.processNextRequest();
  server.handleClient();
  delay(1);
}
