#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>RCV YV4AA - CW Trainer & Decoder</title>
  <style>
    :root {
      --primary: #1f2fae;
      --primary-dark: #152970;
      --accent: #3a4ee0;
      --danger: #d32f2f;
      --bg: #0d0e12;
      --card: #16181f;
      --subtle: #222531;
      --border: #2e3447;
      --text: #f0f2f5;
      --text-muted: #8c93a8;
      --term-green: #4ade80;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: var(--bg); color: var(--text); padding: 12px; display: flex; justify-content: center; }
    .card { background: var(--card); padding: 18px; border-radius: 16px; width: 100%; max-width: 480px; box-shadow: 0 10px 30px rgba(0,0,0,0.7); border: 1px solid var(--border); }
    
    .header { text-align: center; margin-bottom: 14px; background: linear-gradient(135deg, var(--primary-dark), var(--primary)); padding: 14px; border-radius: 12px; display: flex; flex-direction: column; align-items: center; justify-content: center; }
    .logo-img { width: 68px; height: 68px; margin-bottom: 8px; filter: drop-shadow(0 2px 6px rgba(0,0,0,0.4)); }
    .header h1 { font-size: 1.05rem; color: #FFF; letter-spacing: 0.5px; }
    .sub-title { font-size: 0.75rem; color: #a4b1ff; font-weight: bold; margin-top: 2px; }
    
    .tabs { display: grid; grid-template-columns: repeat(5, 1fr); gap: 4px; margin-bottom: 14px; background: var(--subtle); padding: 4px; border-radius: 10px; border: 1px solid var(--border); }
    .tab-btn { background: transparent; border: none; color: var(--text-muted); padding: 8px 2px; border-radius: 6px; font-weight: bold; cursor: pointer; font-size: 0.68rem; transition: 0.2s; }
    .tab-btn.active { background: var(--primary); color: #FFF; box-shadow: 0 2px 8px rgba(31, 47, 174, 0.5); }
    
    .section { margin-bottom: 12px; }
    label { font-size: 0.75rem; font-weight: bold; color: var(--text-muted); display: block; margin-bottom: 5px; }
    input[type=range] { width: 100%; height: 6px; background: var(--subtle); border-radius: 4px; accent-color: var(--accent); margin: 6px 0; }
    .val-display { float: right; color: #a4b1ff; font-weight: bold; }
    input[type=text], input[type=password], select, textarea { width: 100%; background: var(--subtle); border: 1px solid var(--border); border-radius: 8px; color: #FFF; padding: 9px; font-size: 0.85rem; outline: none; }
    input:focus, textarea:focus, select:focus { border-color: var(--accent); }
    
    .btn-group { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; margin-top: 8px; }
    .btn-group-3 { display: grid; grid-template-columns: 1fr 1.3fr 1fr; gap: 6px; margin-top: 8px; }
    button { color: #FFF; border: none; padding: 10px; font-size: 0.82rem; font-weight: bold; border-radius: 8px; cursor: pointer; transition: 0.15s; }
    button:active { transform: scale(0.97); }
    .btn-play { background: linear-gradient(135deg, var(--primary), var(--accent)); }
    .btn-stop { background: var(--danger); }
    .btn-sec { background: var(--subtle); border: 1px solid var(--border); color: #d1d5db; }
    .btn-del { background: #5c1414; border: 1px solid #8b1f1f; color: #ffb4b4; }
    
    .flashcard { background: #08090c; border: 1px solid var(--border); border-radius: 12px; padding: 16px; text-align: center; margin: 10px 0; min-height: 48px; display: flex; align-items: center; justify-content: center; flex-direction: column; }
    .word-text { font-size: 1.7rem; font-weight: 800; color: #FFF; letter-spacing: 2px; }
    .word-count { font-size: 0.72rem; color: var(--text-muted); margin-top: 4px; }
    .blind-hidden { filter: blur(8px); user-select: none; }
    
    .terminal-box { background: #08090c; border: 1px solid var(--primary); border-radius: 8px; padding: 12px; min-height: 90px; max-height: 150px; overflow-y: auto; font-family: 'Courier New', monospace; font-size: 1rem; font-weight: bold; color: var(--term-green); word-break: break-word; text-align: left; box-shadow: inset 0 0 10px rgba(0,0,0,0.8); }
    .cursor { animation: blink 1s infinite; }
    @keyframes blink { 0%, 50% { opacity: 1; } 51%, 100% { opacity: 0; } }
    
    .stats-badge { display: inline-flex; align-items: center; gap: 6px; background: var(--subtle); border: 1px solid var(--border); padding: 5px 10px; border-radius: 6px; font-size: 0.75rem; color: #a4b1ff; font-weight: bold; }
    .stats-badge span { color: var(--term-green); font-size: 0.85rem; }

    .net-box { background: #08090c; border: 1px solid var(--border); border-radius: 8px; padding: 10px; margin-bottom: 12px; font-size: 0.8rem; color: #a4b1ff; }
    .key-grid { display: grid; grid-template-columns: repeat(6, 1fr); gap: 5px; margin-top: 8px; }
    .cw-key { background: var(--subtle); border: 1px solid var(--border); color: #FFF; padding: 9px 0; border-radius: 8px; text-align: center; font-weight: bold; cursor: pointer; display: flex; flex-direction: column; align-items: center; justify-content: center; font-size: 0.85rem; }
    .cw-key span { font-size: 0.6rem; color: var(--text-muted); margin-top: 2px; font-family: monospace; }
    .cw-key:active { background: var(--primary); }
    
    .checkbox-container { display: flex; align-items: center; gap: 8px; margin-top: 10px; font-size: 0.78rem; color: #d1d5db; cursor: pointer; }
    .checkbox-container input { width: 16px; height: 16px; accent-color: var(--accent); cursor: pointer; }
    .morse-legend { background: #08090c; border: 1px solid var(--border); border-radius: 8px; padding: 10px; margin-top: 10px; display: grid; grid-template-columns: repeat(4, 1fr); gap: 4px 12px; font-size: 0.72rem; }
    .morse-legend .lg-item { display: flex; justify-content: space-between; align-items: center; color: #d1d5db; padding: 2px 4px; border-radius: 4px; }
    .morse-legend .lg-item b { color: #FFF; }
    .morse-legend .lg-item span { font-family: monospace; color: var(--term-green); letter-spacing: 1px; }
    .status { text-align: left; font-size: 0.75rem; color: var(--text-muted); flex: 1; }
    .footer { display: flex; align-items: center; gap: 10px; margin-top: 14px; padding-top: 8px; border-top: 1px solid var(--border); }
    .lang-btn { flex-shrink: 0; font-size: 0.75rem; padding: 4px 12px; }
  </style>
</head>
<body>
  <div class="card">
    <div class="header">
      <img src="/logo.svg" alt="Logo RCV" style="width: 68px; height: 68px; margin-bottom: 8px;">
      <h1>RADIO CLUB VENEZOLANO</h1>
      <div class="sub-title">CASA REGIONAL MARACAY • YV4AA</div>
    </div>

    <div class="tabs">
      <button class="tab-btn active" id="tab1Btn" onclick="setTab(1)" data-i18n="tab_list">LISTA</button>
      <button class="tab-btn" id="tab2Btn" onclick="setTab(2)" data-i18n="tab_text">TEXTO</button>
      <button class="tab-btn" id="tab3Btn" onclick="setTab(3)" data-i18n="tab_key">TECLADO</button>
      <button class="tab-btn" id="tab4Btn" onclick="setTab(4)" data-i18n="tab_dec">DECODER</button>
      <button class="tab-btn" id="tab5Btn" onclick="setTab(5)" data-i18n="tab_cfg">⚙ CONFIG</button>
    </div>

    <div id="tab1">
      <div class="section">
        <label data-i18n="list_title">LISTA DE PALABRAS / ENTRENAMIENTO:</label>
        <textarea id="listInput" rows="2">CQ QRM QTH 73 RST 5NN DX YV4AA</textarea>
        <div class="btn-group">
          <button class="btn-sec" onclick="loadList()" data-i18n="list_load">📥 CARGAR</button>
          <button class="btn-sec" onclick="toggleBlind()" id="blindBtn">👁 OCULTO: OFF</button>
        </div>
      </div>
      <div class="flashcard">
        <div class="word-text" id="displayWord" data-i18n="list_ready">LISTO</div>
        <div class="word-count" id="displayCount">0 de 0</div>
      </div>
      <div class="section">
        <label><span data-i18n="list_pause">PAUSA AUTO:</span> <span class="val-display" id="pauseVal">2.0 s</span></label>
        <input type="range" id="pauseSlider" min="0.5" max="5.0" step="0.5" value="2.0" oninput="document.getElementById('pauseVal').innerText=this.value+' s'">
        <label class="checkbox-container">
          <input type="checkbox" id="autoNavPlay">
          <span data-i18n="list_autonav">Auto-reproducir al avanzar/retroceder</span>
        </label>
        <label class="checkbox-container">
          <input type="checkbox" id="shufflePlay" onchange="toggleShuffle()">
          <span data-i18n="list_shuffle">🔀 Reproducir aleatoriamente</span>
        </label>
      </div>
      <div class="btn-group-3">
        <button class="btn-sec" onclick="prevWord()" data-i18n="list_prev">⏮ Ant</button>
        <button class="btn-play" onclick="repeatWord()" data-i18n="list_play">▶ / 🔁 Play</button>
        <button class="btn-sec" onclick="nextWord()" data-i18n="list_next">Sig ⏭</button>
      </div>
      <div class="btn-group" style="margin-top: 8px;">
        <button class="btn-play" id="autoBtn" onclick="toggleAutoPlay()" data-i18n="list_autoplay">⏯ AUTO-PLAY</button>
        <button class="btn-stop" onclick="stopAudio()" data-i18n="list_stop">⏹ PARAR</button>
      </div>
    </div>

    <div id="tab2" style="display: none;">
      <div class="section">
        <label data-i18n="text_title">MENSAJE LIBRE / QSO:</label>
        <textarea id="msgText" rows="4">CQ CQ CQ DE YV4AA K</textarea>
        <div class="btn-group">
          <button class="btn-play" onclick="sendContinuous()" data-i18n="text_send">▶ TRANSMITIR</button>
          <button class="btn-stop" onclick="stopAudio()" data-i18n="text_stop">⏹ DETENER</button>
        </div>
        <div class="btn-group">
          <button class="btn-sec" onclick="setPreset('CQ CQ CQ DE YV4AA K')">PRESET CQ</button>
          <button class="btn-sec" onclick="setPreset('73 DE YV4AA SK')">PRESET 73</button>
        </div>
      </div>
    </div>

    <div id="tab3" style="display: none;">
      <label data-i18n="key_title">TECLADO MORSE DIRECTO:</label>
      <div class="key-grid" id="cwKeyboard"></div>
      <div style="margin-top: 10px;">
        <button class="btn-stop" style="width: 100%;" onclick="stopAudio()" data-i18n="key_stop">⏹ DETENER TONO</button>
      </div>
    </div>

    <div id="tab4" style="display: none;">
      <div class="section">
        <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 6px;">
          <label style="margin: 0;" data-i18n="dec_title">DECODIFICADOR EN VIVO:</label>
          <div style="display: flex; gap: 8px; align-items: center;">
            <div class="stats-badge">🎯 Timing: <span id="liveTiming">--</span>%</div>
            <div class="stats-badge"><span data-i18n="dec_speed">Velocidad</span>: <span id="liveWpm">--</span> WPM</div>
          </div>
        </div>
        <div class="terminal-box" id="decoderBox">
          <span id="decodedText" data-i18n="dec_waiting">Esperando manipulación...</span><span class="cursor">_</span>
        </div>
        <button class="btn-sec" style="width: 100%;" onclick="clearDecoder()" data-i18n="dec_clear">🗑 LIMPIAR</button>
        <div style="margin-top: 10px;">
          <button class="btn-sec" style="width: 100%;" onclick="toggleBlindLegend()" id="legendBlindBtn">👁 OCULTO: OFF</button>
        </div>
        <div class="morse-legend" id="morseLegend"></div>
      </div>
    </div>

    <div id="tab5" style="display: none;">
      <div class="net-box" id="netStatusBox"><span data-i18n="cfg_netcheck">Estado de Red: Consultando...</span></div>
      
      <div class="section">
        <label><span data-i18n="cfg_speed">VELOCIDAD:</span> <span class="val-display" id="wpmVal">15 WPM</span></label>
        <input type="range" id="wpmSlider" min="5" max="40" value="15" oninput="updateWPM(this.value)">
      </div>
      <div class="section">
        <label><span data-i18n="cfg_tone">FRECUENCIA DE TONO:</span> <span class="val-display" id="freqVal">700 Hz</span></label>
        <input type="range" id="freqSlider" min="400" max="1000" step="25" value="700" oninput="updateFreq(this.value)">
      </div>
      <div class="section">
        <label><span data-i18n="cfg_vol">VOLUMEN:</span> <span class="val-display" id="volVal">80%</span></label>
        <input type="range" id="volSlider" min="0" max="100" value="80" oninput="updateVol(this.value)">
      </div>

      <div class="section" style="margin-top: 14px; border-top: 1px solid var(--border); padding-top: 10px;">
        <label data-i18n="cfg_wifi">REDES WI-FI DISPONIBLES:</label>
        <div class="btn-group">
          <button class="btn-sec" onclick="scanWiFi()" data-i18n="cfg_scan">🔍 ESCANEAR</button>
          <select id="scannedNetworks" onchange="document.getElementById('cfgSSID').value=this.value">
            <option value="" data-i18n="cfg_selnet">-- Redes detectadas --</option>
          </select>
        </div>
      </div>
      <div class="section">
        <label data-i18n="cfg_ssid">SSID WI-FI:</label>
        <input type="text" id="cfgSSID" placeholder="Nombre de red">
      </div>
      <div class="section">
        <label data-i18n="cfg_pass">CONTRASEÑA WI-FI:</label>
        <input type="password" id="cfgPass" placeholder="••••••••">
      </div>
      <div class="btn-group">
        <button class="btn-play" onclick="saveConfig()" data-i18n="cfg_save">💾 GUARDAR</button>
        <button class="btn-del" onclick="clearWiFi()" data-i18n="cfg_forget">🗑 OLVIDAR</button>
      </div>
    </div>

    <div class="footer">
      <div class="status" id="status" data-i18n="status_ready">Listo | Modo Dark Activo</div>
      <button class="btn-sec lang-btn" onclick="toggleLang()" id="langBtn">🌐 EN</button>
    </div>
  </div>

  <script>
    // ===================== I18N =====================
    let lang = 'es';
    const I18N = {
      es: {
        tab_list: 'LISTA', tab_text: 'TEXTO', tab_key: 'TECLADO', tab_dec: 'DECODER', tab_cfg: '⚙ CONFIG',
        list_title: 'LISTA DE PALABRAS / ENTRENAMIENTO:', list_load: '📥 CARGAR', list_ready: 'LISTO',
        list_pause: 'PAUSA AUTO:', list_autonav: 'Auto-reproducir al avanzar/retroceder', list_shuffle: '🔀 Reproducir aleatoriamente',
        list_prev: '⏮ Ant', list_play: '▶ / 🔁 Play', list_next: 'Sig ⏭', list_autoplay: '⏯ AUTO-PLAY', list_stop: '⏹ PARAR',
        text_title: 'MENSAJE LIBRE / QSO:', text_send: '▶ TRANSMITIR', text_stop: '⏹ DETENER',
        key_title: 'TECLADO MORSE DIRECTO:', key_stop: '⏹ DETENER TONO',
        dec_title: 'DECODIFICADOR EN VIVO:', dec_speed: 'Velocidad', dec_waiting: 'Esperando manipulación...',
        dec_clear: '🗑 LIMPIAR', dec_copy: '📋 COPIAR',
        cfg_netcheck: 'Estado de Red: Consultando...', cfg_speed: 'VELOCIDAD:', cfg_tone: 'FRECUENCIA DE TONO:', cfg_vol: 'VOLUMEN:',
        cfg_wifi: 'REDES WI-FI DISPONIBLES:', cfg_scan: '🔍 ESCANEAR', cfg_selnet: '-- Redes detectadas --',
        cfg_ssid: 'SSID WI-FI:', cfg_pass: 'CONTRASEÑA WI-FI:', cfg_save: '💾 GUARDAR', cfg_forget: '🗑 OLVIDAR',
        status_ready: 'Listo | Modo Dark Activo',
        blind_on: '👁 OCULTO: ON', blind_off: '👁 OCULTO: OFF',
        s_transmitting: 'Transmitiendo...', s_ready: 'Listo', s_stopped: 'Detenido',
        s_loaded: 'Lista cargada', s_shuffle_on: '🔀 Modo aleatorio: ON', s_shuffle_off: '🔀 Modo aleatorio: OFF',
        s_playing: 'Reproduciendo...', s_scanning: 'Escaneando redes Wi-Fi...', s_scan_done: 'Escaneo listo',
        s_scan_err: 'Error al escanear redes', s_copied: 'Copiado al portapapeles',
        s_saving: 'Guardando configuración...', s_saved: 'Guardado. Reconectando...', s_cleared: 'Red eliminada',
        s_ssid_req: 'Ingresa o selecciona un SSID', s_forget_confirm: '¿Olvidar red Wi-Fi y operar únicamente en modo AP?',
        w_words: 'palabras', w_networks: 'redes'
      },
      en: {
        tab_list: 'LIST', tab_text: 'TEXT', tab_key: 'KEYBOARD', tab_dec: 'DECODER', tab_cfg: '⚙ CONFIG',
        list_title: 'WORD LIST / TRAINING:', list_load: '📥 LOAD', list_ready: 'READY',
        list_pause: 'AUTO PAUSE:', list_autonav: 'Auto-play on next/prev', list_shuffle: '🔀 Shuffle playback',
        list_prev: '⏮ Prev', list_play: '▶ / 🔁 Play', list_next: 'Next ⏭', list_autoplay: '⏯ AUTO-PLAY', list_stop: '⏹ STOP',
        text_title: 'FREE TEXT / QSO:', text_send: '▶ SEND', text_stop: '⏹ STOP',
        key_title: 'DIRECT MORSE KEYBOARD:', key_stop: '⏹ STOP TONE',
        dec_title: 'LIVE DECODER:', dec_speed: 'Speed', dec_waiting: 'Waiting for keying...',
        dec_clear: '🗑 CLEAR', dec_copy: '📋 COPY',
        cfg_netcheck: 'Network Status: Checking...', cfg_speed: 'SPEED:', cfg_tone: 'TONE FREQUENCY:', cfg_vol: 'VOLUME:',
        cfg_wifi: 'AVAILABLE WI-FI NETWORKS:', cfg_scan: '🔍 SCAN', cfg_selnet: '-- Detected networks --',
        cfg_ssid: 'WI-FI SSID:', cfg_pass: 'WI-FI PASSWORD:', cfg_save: '💾 SAVE', cfg_forget: '🗑 FORGET',
        status_ready: 'Ready | Dark Mode Active',
        blind_on: '👁 HIDDEN: ON', blind_off: '👁 HIDDEN: OFF',
        s_transmitting: 'Transmitting...', s_ready: 'Ready', s_stopped: 'Stopped',
        s_loaded: 'List loaded', s_shuffle_on: '🔀 Shuffle: ON', s_shuffle_off: '🔀 Shuffle: OFF',
        s_playing: 'Playing...', s_scanning: 'Scanning Wi-Fi networks...', s_scan_done: 'Scan complete',
        s_scan_err: 'Error scanning networks', s_copied: 'Copied to clipboard',
        s_saving: 'Saving configuration...', s_saved: 'Saved. Reconnecting...', s_cleared: 'Network removed',
        s_ssid_req: 'Enter or select an SSID', s_forget_confirm: 'Forget Wi-Fi and operate in AP mode only?',
        w_words: 'words', w_networks: 'networks'
      }
    };

    function t(key) { return (I18N[lang] && I18N[lang][key]) ? I18N[lang][key] : key; }

    function applyLang() {
      document.querySelectorAll('[data-i18n]').forEach(el => {
        el.innerText = t(el.getAttribute('data-i18n'));
      });
      document.getElementById('langBtn').innerText = lang === 'es' ? '🌐 EN' : '🌐 ES';
      updateBlindLabels();
      const d = document.getElementById('displayWord');
      if (words.length === 0) d.innerText = t('list_ready');
    }

    function toggleLang() {
      lang = (lang === 'es') ? 'en' : 'es';
      try { localStorage.setItem('cw_lang', lang); } catch(e) {}
      applyLang();
      refreshStatus();
    }

    function updateBlindLabels() {
      document.getElementById('blindBtn').innerText = blindMode ? t('blind_on') : t('blind_off');
      document.getElementById('legendBlindBtn').innerText = legendBlind ? t('blind_on') : t('blind_off');
    }

    let words = [], currentIndex = 0, blindMode = false, isAutoPlaying = false, autoTimer = null;
    let shuffleMode = false, playQueue = [], queuePos = 0;   // modo aleatorio (baraja sin repetir)
    let decoderInterval = null;
    let decodedTotal = 0;
    let decoderBusy = false;

    const morseChars = [
      {c:'A',m:'.-'},{c:'B',m:'-...'},{c:'C',m:'-.-.'},{c:'D',m:'-..'},{c:'E',m:'.'},{c:'F',m:'..-.'},
      {c:'G',m:'--.'},{c:'H',m:'....'},{c:'I',m:'..'},{c:'J',m:'.---'},{c:'K',m:'-.-'},{c:'L',m:'.-..'},
      {c:'M',m:'--'},{c:'N',m:'-.'},{c:'O',m:'---'},{c:'P',m:'.--.'},{c:'Q',m:'--.-'},{c:'R',m:'.-.'},
      {c:'S',m:'...'},{c:'T',m:'-'},{c:'U',m:'..-'},{c:'V',m:'...-'},{c:'W',m:'.--'},{c:'X',m:'-..-'},
      {c:'Y',m:'-.--'},{c:'Z',m:'--..'},{c:'1',m:'.----'},{c:'2',m:'..---'},{c:'3',m:'...--'},{c:'4',m:'....-'},
      {c:'5',m:'.....'},{c:'6',m:'-....'},{c:'7',m:'--...'},{c:'8',m:'---..'},{c:'9',m:'----.'},{c:'0',m:'-----'},
      {c:'?',m:'..--..'},{c:'/',m:'-..-.'},{c:'=',m:'-...-'},{c:',',m:'--..--'},{c:'.',m:'.-.-.-'},{c:'SK',m:'...-.-'}
    ];

    function initKeyboard() {
      const grid = document.getElementById('cwKeyboard');
      grid.innerHTML = '';
      morseChars.forEach(item => {
        const btn = document.createElement('div');
        btn.className = 'cw-key';
        btn.innerHTML = `${item.c}<span>${item.m}</span>`;
        btn.onclick = () => playChar(item.c);
        grid.appendChild(btn);
      });
    }

    function setTab(num) {
      for(let i=1; i<=5; i++) {
        document.getElementById('tab'+i).style.display = (num === i) ? 'block' : 'none';
        document.getElementById('tab'+i+'Btn').className = 'tab-btn ' + ((num === i) ? 'active' : '');
      }
      stopAudio();

      if (num === 4) {
        fetch('/set_decoder?state=1');
        startDecoderPolling();
      } else {
        stopDecoderPolling();
        fetch('/set_decoder?state=0');
      }

      if(num === 5) refreshStatus();
    }

    function startDecoderPolling() {
      if (decoderInterval) clearInterval(decoderInterval);
      decoderBusy = false;
      decoderInterval = setInterval(() => {
        // v12.1: flag busy -> nunca lanzar un fetch si el anterior sigue en
        // vuelo. Antes, con el servidor lento, 2-4 fetches con el mismo since
        // se solapaban y cada uno apendaba el mismo delta (letras x4).
        if (decoderBusy) return;
        decoderBusy = true;
        fetch('/get_decoded?since=' + decodedTotal)
          .then(r => r.json())
          .then(d => {
            const box = document.getElementById('decodedText');
            const waiting = t('dec_waiting');
            if (d.n !== undefined && d.n < decodedTotal) decodedTotal = 0; // servidor reinició
            // Solo apendar si hay texto NUEVO (d.n > decodedTotal) o si el
            // servidor mandó el buffer completo (full) o el cuadro está vacío.
            if (d.text && d.text.length > 0 && (d.full || d.n > decodedTotal || box.innerText === waiting)) {
              if (d.full || box.innerText === waiting) {
                box.innerText = d.text;
              } else {
                box.innerText += d.text;
              }
            } else if (decodedTotal === 0) {
              // Solo mostrar "Esperando..." si NO hay nada decodificado.
              box.innerText = waiting;
            }
            if (d.n !== undefined) decodedTotal = d.n;
            if (d.wpm !== undefined) {
              document.getElementById('liveWpm').innerText = d.wpm > 0 ? d.wpm : '--';
            }
            if (d.timing !== undefined) {
              const t = document.getElementById('liveTiming');
              t.innerText = d.timing;
              t.style.color = d.timing >= 80 ? '#2ecc71' : (d.timing >= 50 ? '#f1c40f' : '#e74c3c');
            }
          }).catch(() => {})
          .finally(() => { decoderBusy = false; });
      }, 250);
    }

    function stopDecoderPolling() {
      if (decoderInterval) {
        clearInterval(decoderInterval);
        decoderInterval = null;
      }
    }

    function updateWPM(val) { document.getElementById('wpmVal').innerText = val + ' WPM'; fetch('/set_wpm?val=' + val); }
    function updateFreq(val) { document.getElementById('freqVal').innerText = val + ' Hz'; fetch('/set_freq?val=' + val); }
    function updateVol(val) { document.getElementById('volVal').innerText = val + '%'; fetch('/set_vol?val=' + val); }

    function sendContinuous() {
      const text = document.getElementById('msgText').value;
      setStatus(t('s_transmitting'), '#3a4ee0');
      fetch('/play?text=' + encodeURIComponent(text)).then(() => setStatus(t('s_ready'), '#8c93a8'));
    }
    function setPreset(txt) { document.getElementById('msgText').value = txt; setStatus(t('s_ready'), '#8c93a8'); }
    function playChar(ch) { fetch('/play?text=' + encodeURIComponent(ch)); }

    let legendBlind = false;
    function toggleBlindLegend() {
      legendBlind = !legendBlind;
      updateBlindLabels();
      document.getElementById('morseLegend').classList.toggle('blind-hidden', legendBlind);
    }

    function buildLegend() {
      const lg = document.getElementById('morseLegend');
      lg.innerHTML = '';
      morseChars.forEach(item => {
        const d = document.createElement('div');
        d.className = 'lg-item';
        d.innerHTML = `<b>${item.c}</b><span>${item.m}</span>`;
        lg.appendChild(d);
      });
    }

    function stopAudio() {
      isAutoPlaying = false;
      clearTimeout(autoTimer);
      document.getElementById('autoBtn').innerText = t('list_autoplay');
      fetch('/stop').then(() => setStatus(t('s_stopped'), '#d32f2f'));
    }

    function setStatus(msg, color) {
      const st = document.getElementById('status');
      st.innerText = msg;
      st.style.color = color;
    }

    function loadList() {
      const raw = document.getElementById('listInput').value;
      words = raw.trim().split(/[\s,]+/);
      currentIndex = 0;
      buildQueue();
      updateCard();
      setStatus(t('s_loaded') + ' (' + words.length + ' ' + t('w_words') + ')', '#7d8eff');
    }

    // Construye la cola de reproducción: orden natural o barajada (sin repetir)
    function buildQueue() {
      playQueue = [];
      for (let i = 0; i < words.length; i++) playQueue.push(i);
      if (shuffleMode) {
        // Fisher-Yates
        for (let i = playQueue.length - 1; i > 0; i--) {
          const j = Math.floor(Math.random() * (i + 1));
          [playQueue[i], playQueue[j]] = [playQueue[j], playQueue[i]];
        }
      }
      queuePos = 0;
      if (playQueue.length > 0) currentIndex = playQueue[0];
    }

    function toggleShuffle() {
      shuffleMode = document.getElementById('shufflePlay').checked;
      if (words.length > 0 && !isAutoPlaying) buildQueue();
      setStatus(shuffleMode ? t('s_shuffle_on') : t('s_shuffle_off'), '#7d8eff');
    }

    // Avanza a la siguiente palabra (respeta la cola actual)
    function advanceIndex() {
      if (playQueue.length === 0) return false;
      queuePos++;
      if (queuePos >= playQueue.length) return false;  // lista agotada
      currentIndex = playQueue[queuePos];
      return true;
    }

    function updateCard() {
      if (words.length === 0) return;
      const el = document.getElementById('displayWord');
      el.innerText = words[currentIndex];
      el.className = 'word-text ' + (blindMode ? 'blind-hidden' : '');
      const shown = queuePos + 1;
      document.getElementById('displayCount').innerText = (shuffleMode ? '🔀 ' : '') + shown + ' / ' + words.length;
    }

    function toggleBlind() {
      blindMode = !blindMode;
      updateBlindLabels();
      updateCard();
    }

    function playCurrentWord(onDone) {
      if (words.length === 0) loadList();
      updateCard();
      // No revelar la palabra en el status (respeta el modo oculto 👁)
      setStatus(t('s_playing'), '#3a4ee0');
      fetch('/play?text=' + encodeURIComponent(words[currentIndex]))
        .then(() => {
          setStatus(t('s_ready'), '#8c93a8');
          if (onDone) onDone();
        });
    }

    function repeatWord() { playCurrentWord(); }

    function nextWord() {
      if (words.length === 0) loadList();
      if (advanceIndex()) {
        updateCard();
        if (document.getElementById('autoNavPlay').checked) playCurrentWord();
      }
    }

    function prevWord() {
      if (words.length === 0) loadList();
      if (queuePos > 0) {
        queuePos--;
        currentIndex = playQueue[queuePos];
        updateCard();
        if (document.getElementById('autoNavPlay').checked) playCurrentWord();
      }
    }

    function toggleAutoPlay() {
      if (isAutoPlaying) {
        stopAudio();
      } else {
        if (words.length === 0) loadList();
        isAutoPlaying = true;
        document.getElementById('autoBtn').innerText = '⏸ ' + (lang === 'es' ? 'PAUSAR' : 'PAUSE');
        runAutoLoop();
      }
    }

    function runAutoLoop() {
      if (!isAutoPlaying) return;
      playCurrentWord(() => {
        if (!isAutoPlaying) return;
        if (advanceIndex()) {
          const pause = parseFloat(document.getElementById('pauseSlider').value) * 1000;
          autoTimer = setTimeout(() => {
            updateCard();
            runAutoLoop();
          }, pause);
        } else {
          // NUNCA parar: al agotar la lista, re-barajar (o reiniciar en orden) y seguir
          buildQueue();
          const pause = parseFloat(document.getElementById('pauseSlider').value) * 1000;
          autoTimer = setTimeout(() => {
            updateCard();
            runAutoLoop();
          }, pause);
        }
      });
    }

    function clearDecoder() {
      fetch('/clear_decoded').then(() => {
        document.getElementById('decodedText').innerText = t('dec_waiting');
        document.getElementById('liveWpm').innerText = "--";
        decodedTotal = 0;
      });
    }

    function scanWiFi() {
      setStatus(t('s_scanning'), '#3a4ee0');
      fetch('/scan_wifi')
        .then(res => res.json())
        .then(networks => {
          const sel = document.getElementById('scannedNetworks');
          sel.innerHTML = '<option value="">' + t('cfg_selnet') + '</option>';
          networks.forEach(net => {
            const opt = document.createElement('option');
            opt.value = net.ssid;
            opt.innerText = net.ssid + ' (' + net.rssi + ' dBm)';
            sel.appendChild(opt);
          });
          setStatus(t('s_scan_done') + ' (' + networks.length + ' ' + t('w_networks') + ')', '#7d8eff');
        })
        .catch(() => setStatus(t('s_scan_err'), '#d32f2f'));
    }

    function refreshStatus() {
      fetch('/get_status')
        .then(res => res.json())
        .then(data => {
          document.getElementById('netStatusBox').innerHTML = data.wifi_status;
        });
    }

    function saveConfig() {
      const ssid = document.getElementById('cfgSSID').value;
      const pass = document.getElementById('cfgPass').value;
      if (!ssid) { alert(t('s_ssid_req')); return; }
      setStatus(t('s_saving'), '#3a4ee0');
      fetch('/save_cfg?ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass))
        .then(() => {
          setStatus(t('s_saved'), '#7d8eff');
          setTimeout(refreshStatus, 3000);
        });
    }

    function clearWiFi() {
      if (confirm(t('s_forget_confirm'))) {
        fetch('/clear_wifi').then(() => {
          document.getElementById('cfgSSID').value = '';
          document.getElementById('cfgPass').value = '';
          setStatus(t('s_cleared'), '#d32f2f');
          setTimeout(refreshStatus, 1500);
        });
      }
    }

    window.onload = () => {
      // Idioma: preferencia guardada > idioma del navegador > español
      try {
        const saved = localStorage.getItem('cw_lang');
        if (saved === 'en' || saved === 'es') lang = saved;
        else lang = (navigator.language || 'es').toLowerCase().startsWith('es') ? 'es' : 'en';
      } catch(e) {}
      applyLang();
      initKeyboard();
      loadList();
      refreshStatus();
      buildLegend();
      // Pause the decoder if we don't start on the DECODER tab
      // Si no arrancamos en la pestaña DECODER, pausar el decoder
      fetch('/set_decoder?state=0');
    };

    // Pause the decoder when leaving/closing the page
    // Al cerrar/abandonar la página, pausar el decoder
    window.addEventListener('beforeunload', () => {
      if (navigator.sendBeacon) {
        navigator.sendBeacon('/set_decoder?state=0');
      } else {
        fetch('/set_decoder?state=0');
      }
    });
  </script>
</body>
</html>
)rawliteral";

#endif
