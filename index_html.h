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
    
    .tabs { display: grid; grid-template-columns: repeat(6, 1fr); gap: 4px; margin-bottom: 14px; background: var(--subtle); padding: 4px; border-radius: 10px; border: 1px solid var(--border); }
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
    /* 🎮 JUEGO v13 */
    .game-hud { display: flex; justify-content: space-between; align-items: center; font-size: 0.75rem; color: var(--text-muted); margin-bottom: 8px; gap: 6px; flex-wrap: wrap; }
    .game-letter { font-size: 5rem; font-weight: 900; text-align: center; color: #4a5cf0; letter-spacing: 4px; opacity: 0.5; transition: all 0.3s ease; margin: 10px 0; user-select: none; }
    .game-letter.crisp { filter: none; opacity: 1; color: #FFF; }
    .game-letter.crisp-ok { filter: none; opacity: 1; color: #2ecc71; }
    .game-letter.crisp-bad { filter: none; opacity: 1; color: #e74c3c; }
    .game-options { display: grid; grid-template-columns: repeat(3, 1fr); gap: 8px; margin: 10px 0; }
    .game-opt { padding: 14px 0; font-size: 1.4rem; font-weight: 800; border-radius: 10px; border: 1px solid var(--border); background: var(--subtle); color: #FFF; cursor: pointer; transition: 0.15s; }
    .game-opt:disabled { opacity: 0.45; cursor: default; }
    .game-opt.right { background: #1e7a34; border-color: #2ecc71; }
    .game-opt.wrong { background: #7a1e1e; border-color: #e74c3c; }
    .game-hint { text-align: center; font-size: 0.85rem; color: var(--term-green); min-height: 1.3em; margin-top: 6px; }
    .game-sub { text-align: center; font-size: 0.85rem; color: var(--text-muted); min-height: 1.3em; margin-top: 4px; }
    .btn-active { background: var(--primary) !important; color: #FFF !important; }
    .weak-title { font-size: 0.7rem; color: var(--text-muted); margin-top: 14px; }
    .weak-grid { display: grid; grid-template-columns: repeat(7, 1fr); gap: 4px; margin-top: 6px; }
    .weak-cell { text-align: center; font-size: 0.8rem; font-weight: 700; padding: 3px 0; border-radius: 6px; border: 1px solid var(--border); }
    .weak-cell i { display: block; font-size: 0.55rem; font-style: normal; opacity: 0.75; }
    .weak-cell.w-m { background: rgba(46,204,113,0.18); color: #2ecc71; }
    .weak-cell.w-t { background: rgba(241,196,15,0.18); color: #f1c40f; }
    .weak-cell.w-n { background: rgba(140,147,168,0.12); color: #8c93a8; }
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
      <button class="tab-btn" id="tab5Btn" onclick="setTab(5)" data-i18n="tab_game">🎮 JUEGO</button>
      <button class="tab-btn" id="tab6Btn" onclick="setTab(6)" data-i18n="tab_cfg">⚙ CONFIG</button>
    </div>

    <div id="tab1">
      <div class="section">
        <label data-i18n="list_title">LISTA DE PALABRAS / ENTRENAMIENTO:</label>
        <textarea id="listInput" rows="3">QRM QRN QRO QRP QRT QRZ QSB QSL QSO QSY QTH QRK QRL QRS QRX QTC QTR</textarea>
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

    <div id="tab6" style="display: none;">
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
        <label><span data-i18n="cfg_audio_out">SALIDA DE AUDIO:</span></label>
        <select id="audioModeSel" onchange="setAudioMode(this.value)">
          <option value="0" data-i18n="out_buzzer">🔔 SOLO BUZZER</option>
          <option value="1" data-i18n="out_aux">🎧 SOLO AUX</option>
          <option value="2" data-i18n="out_both" selected>🔔🎧 BUZZER + AUX</option>
        </select>
      </div>
      <div class="section">
        <label><span data-i18n="cfg_vol_buzzer">VOL BUZZER:</span> <span class="val-display" id="volBuzzerVal">80%</span></label>
        <input type="range" id="volBuzzerSlider" min="0" max="100" value="80" oninput="updateVolBuzzer(this.value)">
      </div>
      <div class="section">
        <label><span data-i18n="cfg_vol_aux">VOL AUX:</span> <span class="val-display" id="volAuxVal">80%</span></label>
        <input type="range" id="volAuxSlider" min="0" max="100" value="80" oninput="updateVolAux(this.value)">
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

    <div id="tab5" style="display: none;">
      <!-- 🎮 JUEGO (v13.1: tab 5; CONFIG pasó a tab 6) -->
      <div id="gameCfg">
        <div class="section">
          <label data-i18n="g_mode">MODO:</label>
          <div class="btn-group">
            <button class="btn-sec" id="gModeLec" onclick="setGameMode('lec')">👂 <span data-i18n="g_mode_lec">LECTURA</span></button>
            <button class="btn-sec" id="gModeEsc" onclick="setGameMode('esc')">✊ <span data-i18n="g_mode_esc">ESCRITURA</span></button>
          </div>
        </div>
        <div class="section">
          <label data-i18n="g_set">SET DE CARACTERES:</label>
          <div class="btn-group">
            <button class="btn-sec" id="gSetLetters" onclick="setGameSet('letters')">🔤 <span data-i18n="g_set_letters">LETRAS</span></button>
            <button class="btn-sec" id="gSetNumbers" onclick="setGameSet('numbers')">🔢 <span data-i18n="g_set_numbers">NÚMEROS</span></button>
            <button class="btn-sec" id="gSetSigns" onclick="setGameSet('signs')">✳️ <span data-i18n="g_set_signs">SIGNOS</span></button>
            <button class="btn-sec" id="gSetAdvanced" onclick="setGameSet('advanced')">🚀 <span data-i18n="g_set_advanced">AVANZADO</span></button>
          </div>
        </div>
        <div class="section">
          <label class="checkbox-container">
            <input type="checkbox" id="gKoch" onchange="toggleGameKoch()">
            <span data-i18n="g_koch">Progresión estricta Koch (5 letras, +1 por ronda)</span>
          </label>
          <div class="game-sub" id="gKochInfo" style="text-align:left; margin-top:2px;"></div>
        </div>
        <div class="section">
          <label data-i18n="g_rounds">RONDAS POR SESIÓN:</label>
          <input type="number" id="gRoundsInput" min="1" max="200" value="20" inputmode="numeric" style="width:100%; padding:10px; font-size:1.05em; text-align:center; box-sizing:border-box;">
        </div>
        <button class="btn-play" style="width:100%; margin-top:4px;" onclick="gameStart()">🚀 <span data-i18n="g_start">INICIAR</span></button>
        <div id="gameWeakBox"></div>
        <button class="btn-del" style="width:100%; margin-top:8px;" onclick="gameResetStats()">🗑 <span data-i18n="g_reset">REINICIAR PROGRESO</span></button>
      </div>

      <!-- 🎮 PARTIDA -->
      <div id="gamePlay" style="display:none;">
        <div class="game-hud">
          <span><span data-i18n="g_round">Ronda</span> <span id="gPos">1/20</span></span>
          <span>✅ <span id="gScore">0</span></span>
          <span>🔥 <span id="gStreak">0</span></span>
          <span id="gTimingWrap" style="display:none">🎯 <span id="gTiming">--</span>%</span>
          <span id="gDiscWrap" style="display:none" title="descartados">🚫 <span id="gDiscarded">0</span></span>
        </div>
        <div id="gLecView">
          <div class="game-letter" id="gLetter">?</div>
          <div class="game-options" id="gOpts"></div>
          <div class="game-hint" id="gHint"></div>
          <div class="btn-group" style="margin-top:4px;">
            <button class="btn-sec" onclick="gameRepeat()">🔁 <span data-i18n="g_repeat">REPETIR</span></button>
            <button class="btn-sec" onclick="gameGiveUp()">😅 <span data-i18n="g_giveup">NO SÉ → MOSTRAR</span></button>
          </div>
        </div>
        <div id="gEscView" style="display:none">
          <div class="game-letter crisp" id="gEscLetter">A</div>
          <div class="game-sub" id="gEscSub"></div>
          <div class="game-hint" id="gEscHint"></div>
          <div class="game-hint" id="gEscOwnerHint" style="display:none; color:#e74c3c;"></div>
          <button class="btn-sec" style="width:100%; margin-top:4px;" onclick="gameDontKnow()">😅 <span data-i18n="g_dontknow">NO SÉ → COPIAR</span></button>
        </div>
        <div id="gameDone" style="display:none; text-align:center;">
          <div class="word-text" style="filter:none; margin-top:10px;">🎉 <span data-i18n="g_done">Ronda completa</span></div>
          <div style="display:flex; gap:8px; justify-content:center; margin-top:10px;">
            <div class="stats-badge">✅ <span id="gDoneScore">0</span></div>
            <div class="stats-badge">🔥 <span data-i18n="g_best">Mejor racha:</span> <span id="gDoneBest">0</span></div>
          </div>
          <div id="gDoneWeak"></div>
          <button id="gRepeatRoundBtn" class="btn-sec" style="width:100%; margin-top:12px;" onclick="gameRepeatRound()">🔁 <span data-i18n="g_repeat_round">REPETIR RONDA</span></button>
          <button class="btn-play" style="width:100%; margin-top:8px;" onclick="gameNextRound()">▶ <span data-i18n="g_next_round">SIGUIENTE RONDA</span></button>
        </div>
        <button class="btn-stop" style="width:100%; margin-top:10px;" onclick="gameQuit()">⏹ <span data-i18n="g_quit">SALIR A CONFIGURACIÓN</span></button>
      </div>
    </div>

    <div class="footer">
      <div class="status" id="status" data-i18n="status_ready">Listo | Modo Dark Activo</div>
      <div class="status" id="verTag" style="opacity:.55; font-size:11px;">v13.15 · 2026-09-23</div>
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
        cfg_netcheck: 'Estado de Red: Consultando...', cfg_speed: 'VELOCIDAD:', cfg_tone: 'FRECUENCIA DE TONO:', cfg_vol_buzzer: 'VOL BUZZER:', cfg_vol_aux: 'VOL AUX:', cfg_audio_out: 'SALIDA DE AUDIO:', out_buzzer: '🔔 SOLO BUZZER', out_aux: '🎧 SOLO AUX', out_both: '🔔🎧 BUZZER + AUX',
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
        w_words: 'palabras', w_networks: 'redes',
        tab_game: '🎮 JUEGO',
        g_mode: 'MODO:', g_mode_lec: 'LECTURA', g_mode_esc: 'ESCRITURA',
        g_set: 'SET DE CARACTERES:', g_set_letters: 'LETRAS', g_set_numbers: 'NÚMEROS', g_set_signs: 'SIGNOS', g_set_advanced: 'AVANZADO (TODO)',
        g_koch: 'Progresión estricta Koch (5 letras, +1 por ronda)', g_koch_info: 'Nivel {n} · Activas: {s} — +1 letra por ronda completada', g_rounds: 'RONDAS POR SESIÓN:',
        g_start: 'INICIAR', g_reset: 'REINICIAR PROGRESO', g_reset_confirm: '¿Borrar todo el progreso del juego?',
        g_weak_title: 'ESTADO POR CARÁCTER (verde=dominado · amarillo=entrena · gris=nuevo):',
        g_round: 'Ronda', g_key_hint: 'Teclea el código con la llave…',
        g_right: '¡Correcto!', g_answer: 'Era:', g_clue_slow: 'Más lento…', g_clue_pattern: 'Pista:',
        g_giveup: 'NO SÉ → MOSTRAR', g_repeat: 'REPETIR', g_esc_was: 'Eso fue:', g_esc_clue: 'Pista:', g_esc_invalid: 'Código no reconocido. Revisa los espacios entre elementos.',
        g_copy: '🎧 Ahora cópialo con la llave:', g_copy_wrong: 'No era eso. Escucha otra vez:', g_copy_done: '¡Copiado correctamente!',
        g_dontknow: 'NO SÉ → COPIAR', g_quit: 'SALIR A CONFIGURACIÓN', g_owned: '⚠ Otra pestaña/dispositivo tomó el decoder. Reintentando… se recupera solo.', g_discarded: 'descartados',
        g_done: '🎉 Ronda completa', g_best: 'Mejor racha:', g_repeat_round: 'REPETIR RONDA', g_next_round: 'SIGUIENTE RONDA',
        g_need_chars: 'Selecciona al menos 2 caracteres'
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
        cfg_netcheck: 'Network Status: Checking...', cfg_speed: 'SPEED:', cfg_tone: 'TONE FREQUENCY:', cfg_vol_buzzer: 'BUZZER VOL:', cfg_vol_aux: 'AUX VOL:', cfg_audio_out: 'AUDIO OUTPUT:', out_buzzer: '🔔 BUZZER ONLY', out_aux: '🎧 AUX ONLY', out_both: '🔔🎧 BUZZER + AUX',
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
        w_words: 'words', w_networks: 'networks',
        tab_game: '🎮 GAME',
        g_mode: 'MODE:', g_mode_lec: 'LISTENING', g_mode_esc: 'SENDING',
        g_set: 'CHARACTER SET:', g_set_letters: 'LETTERS', g_set_numbers: 'NUMBERS', g_set_signs: 'SIGNS', g_set_advanced: 'ADVANCED (ALL)',
        g_koch: 'Strict Koch progression (5 letters, +1 per round)', g_koch_info: 'Level {n} · Active: {s} — +1 letter per completed round', g_rounds: 'ROUNDS PER SESSION:',
        g_start: 'START', g_reset: 'RESET PROGRESS', g_reset_confirm: 'Delete all game progress?',
        g_weak_title: 'PER-CHARACTER STATUS (green=mastered · yellow=training · gray=new):',
        g_round: 'Round', g_key_hint: 'Key the code with the straight key…',
        g_right: 'Correct!', g_answer: 'It was:', g_clue_slow: 'Slower…', g_clue_pattern: 'Hint:',
        g_giveup: 'I DON\'T KNOW → SHOW', g_repeat: 'REPEAT', g_esc_was: 'That was:', g_esc_clue: 'Hint:', g_esc_invalid: 'Unrecognized code. Check spacing between elements.',
        g_copy: '🎧 Now copy it with the key:', g_copy_wrong: 'Not that. Listen again:', g_copy_done: 'Copied correctly!',
        g_dontknow: 'I DON\'T KNOW → COPY', g_quit: 'EXIT TO CONFIG', g_owned: '⚠ Another tab/device took the decoder. Retrying… it will reconnect automatically.', g_discarded: 'discarded',
        g_done: '🎉 Round complete', g_best: 'Best streak:', g_repeat_round: 'REPEAT ROUND', g_next_round: 'NEXT ROUND',
        g_need_chars: 'Select at least 2 characters'
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
      for(let i=1; i<=6; i++) {
        document.getElementById('tab'+i).style.display = (num === i) ? 'block' : 'none';
        document.getElementById('tab'+i+'Btn').className = 'tab-btn ' + ((num === i) ? 'active' : '');
      }
      stopAudio();

      if (num === 4) {
        // v13.12: sin cerrojo — entrar al decoder lo enciende (v13.9).
        fetch('/set_decoder?state=1');
        startDecoderPolling();
      } else {
        stopDecoderPolling();
        // v13.12: sin cerrojo — al salir del decoder se pausa (v13.9); el
        // juego lo re-encenderá solo al entrar en modo escritura.
        fetch('/set_decoder?state=0');
      }
      // v13.1: el juego (tab 5) usa su propio polling de escritura
      if (num === 5) {
        if (game.phase === 'play' && game.mode === 'esc' && game.escActive) startEscPolling();
        if (game.phase === 'test' && game.mode === 'esc') startEscPolling();
        // v13.12: al volver con carta de escritura en curso, re-encender y
        // re-sincronizar (v13.10/11: quedaba muerto o reclamaba un cerrojo).
        if (game.phase === 'play' && game.mode === 'esc' && !game.escActive) {
          fetch('/set_decoder?state=1&reset=1').catch(() => {});
          game.escActive = true;
          startEscPolling();
        }
        renderWeakBox();
      } else {
        stopEscPolling();
        game.escActive = false;
      }

      if(num === 6) refreshStatus();
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
    function updateVolBuzzer(val) { document.getElementById('volBuzzerVal').innerText = val + '%'; fetch('/set_vol_buzzer?val=' + val); }
    function updateVolAux(val) { document.getElementById('volAuxVal').innerText = val + '%'; fetch('/set_vol_aux?val=' + val); }
    function setAudioMode(val) { fetch('/set_audio_mode?val=' + val); }

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
          // v12.5: sincronizar selector de salida y volúmenes con el firmware
          const am = document.getElementById('audioModeSel');
          if (am && data.audio_mode !== undefined) am.value = String(data.audio_mode);
          if (data.vol_buzzer !== undefined) {
            document.getElementById('volBuzzerSlider').value = data.vol_buzzer;
            document.getElementById('volBuzzerVal').innerText = data.vol_buzzer + '%';
          }
          if (data.vol_aux !== undefined) {
            document.getElementById('volAuxSlider').value = data.vol_aux;
            document.getElementById('volAuxVal').innerText = data.vol_aux + '%';
          }
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

    // ===================== 🎮 JUEGO v13 (2026-08-26) =====================
    // Modos: Lectura (reconocer el código que suena) y Escritura (enviar con
    // la llave física). Progresión adaptativa: test de ubicación + set
    // dinámico (dominadas/entrenamiento/nuevas) con stats por carácter en
    // localStorage. El avanzado no vuelve a empezar: arranca donde está.
    const GAME_GROUPS = { letters: 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', numbers: '0123456789', signs: '.,?/=' };
    // v13.1: progresión Koch por NÚMERO DE PULSACIONES:
    // 2 elementos (A I M N) → 3 (D G K O R S U W) → 4 (B C F H J L P Q V X Y Z)
    // → 1 (E T, triviales) → números (5) → signos.
    const GAME_KOCH_ORDER = 'AIMNDGKORSUWBCFHLJPQVXYZET1234567890.,?/=';
    const GAME_STATS_KEY = 'cw_game_stats';
    const GAME_CFG_KEY = 'cw_game_cfg';
    const GAME_KOCH_KEY = 'cw_game_koch';

    const morseMap = {};
    morseChars.forEach(it => { morseMap[it.c] = it.m; });

    let game = {
      phase: 'cfg', mode: 'lec', set: 'advanced', koch: false, rounds: 20,
      round: [], pos: 0, attempts: 0, copy: false,
      score: 0, streak: 0, bestStreak: 0,
      stats: {}, escMirror: '', escBusy: false, escTimer: null, escActive: false,
      escTotal: 0, escDiscarded: 0, escLastChar: 0,
      playInFlight: false, playLastMs: 0,
      deck: [], deckPos: 0, deckKey: '',
      advTimer: null
    };

    function shuffleArray(a) {
      for (let i = a.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [a[i], a[j]] = [a[j], a[i]];
      }
      return a;
    }

    // ---------- persistencia ----------
    function loadGameStats() { try { game.stats = JSON.parse(localStorage.getItem(GAME_STATS_KEY)) || {}; } catch (e) { game.stats = {}; } }
    function saveGameStats() { try { localStorage.setItem(GAME_STATS_KEY, JSON.stringify(game.stats)); } catch (e) {} }
    function loadGameCfg() {
      try {
        const c = JSON.parse(localStorage.getItem(GAME_CFG_KEY));
        if (c) { game.mode = c.mode || 'lec'; game.set = c.set || 'advanced'; game.koch = !!c.koch; game.rounds = clampRounds(c.rounds || 20); }
      } catch (e) {}
    }
    function saveGameCfg() {
      try { localStorage.setItem(GAME_CFG_KEY, JSON.stringify({ mode: game.mode, set: game.set, koch: game.koch, rounds: game.rounds })); } catch (e) {}
    }
    function getKochLevel() { try { return parseInt(localStorage.getItem(GAME_KOCH_KEY)) || 5; } catch (e) { return 5; } }
    function setKochLevel(l) { try { localStorage.setItem(GAME_KOCH_KEY, String(l)); } catch (e) {} }

    // ---------- selección de set ----------
    function selectedChars(ignoreKoch) {
      let chars = '';
      if (game.set === 'letters') chars = GAME_GROUPS.letters;
      else if (game.set === 'numbers') chars = GAME_GROUPS.numbers;
      else if (game.set === 'signs') chars = GAME_GROUPS.signs;
      else if (game.set === 'advanced') chars = GAME_GROUPS.letters + GAME_GROUPS.numbers + GAME_GROUPS.signs;
      let list = chars.split('');
      if (game.koch && !ignoreKoch) {
        const lv = getKochLevel();
        const k = GAME_KOCH_ORDER.slice(0, lv).split('').filter(c => chars.indexOf(c) >= 0);
        // v13.1: si el nivel Koch no cubre el set elegido (ej: Números con
        // nivel bajo) usar el set completo en vez de error "<2 caracteres".
        if (k.length >= 2) list = k;
      }
      return list;
    }

    // ---------- estado por carácter ----------
    function accOf(ch) { const s = game.stats[ch]; return (s && s.v > 0) ? s.h / s.v : 0; }
    function stateOf(ch) {
      const s = game.stats[ch];
      if (!s || s.v === 0) return 'n';                   // nunca visto
      if (s.v === 1) return (s.h > 0) ? 't' : 'n';       // 1 vista: acierto→entrenando, fallo→nueva
      return (accOf(ch) >= 0.8 || s.r >= 3) ? 'm' : 't'; // dominada / entrenamiento
    }
    function registerHit(ch) {
      const s = game.stats[ch] || { v: 0, h: 0, f: 0, r: 0 };
      s.v++; s.h++; s.r++;
      game.stats[ch] = s;
      game.score++; game.streak++;
      if (game.streak > game.bestStreak) game.bestStreak = game.streak;
      saveGameStats();
      updateHud();
    }
    function registerFail(ch) {
      const s = game.stats[ch] || { v: 0, h: 0, f: 0, r: 0 };
      s.v++; s.f++; s.r = 0;
      game.stats[ch] = s;
      game.streak = 0;
      saveGameStats();
      updateHud();
      // v13.8: la letra fallada vuelve al mazo adelante → repite rápido
      // (en la siguiente ronda). Fallar más = repetir más (adaptividad
      // natural sin romper la cobertura de las demás).
      requeueFailedChar(ch);
    }
    // v13.8: la letra fallada repite RÁPIDO — se reinserta 2 posiciones
    // adelante en la ronda ACTUAL (la ves de nuevo en ~2 cartas). La carta
    // desplazada al final vuelve al mazo (ninguna letra se pierde). Si la
    // ronda está por terminar, cae al mazo para la siguiente.
    function requeueFailedChar(ch) {
      if (game.phase === 'play' && game.round && game.pos < game.round.length - 1) {
        const insertAt = game.pos + 2;
        if (insertAt < game.round.length) {
          // no duplicar si ya viene en las próximas 2 cartas
          if (game.round[game.pos + 1] === ch || game.round[insertAt] === ch) return;
          const dropped = game.round[game.round.length - 1];
          for (let i = game.round.length - 1; i > insertAt; i--) game.round[i] = game.round[i - 1];
          game.round[insertAt] = ch;
          requeueIntoDeck(dropped);
          return;
        }
      }
      requeueIntoDeck(ch);
    }
    // Reinserción en el mazo (siguiente ronda). Límite anti-cascada: máximo
    // 1 copia extra por letra — fallar mucho NO infla el mazo ni retrasa a
    // las demás.
    function requeueIntoDeck(ch) {
      if (!game.deck || game.deckPos >= game.deck.length) return;
      for (let i = game.deckPos; i < game.deck.length; i++) {
        if (game.deck[i] === ch) return;
      }
      let pos = game.deckPos + 2;
      if (pos > game.deck.length) pos = game.deck.length;
      game.deck.splice(pos, 0, ch);
    }

    // ---------- utilidades ----------
    function gameWpm() {
      const v = parseInt(document.getElementById('wpmSlider').value);
      return (v >= 5 && v <= 45) ? v : 15;
    }
    function morseDurationMs(code, wpm) {
      const dit = 1200 / wpm;
      let d = 0;
      for (let i = 0; i < code.length; i++) {
        d += (code[i] === '.') ? dit : dit * 3;
        if (i < code.length - 1) d += dit;
      }
      return d + dit * 2;
    }
    function playLetter(ch, wpm, force) {
      // v13.10: debounce de reproducción — evita sonidos encadenados cuando se
      // pulsa REPETIR varias veces seguidas.
      // v13.14: el sonido de una CARTA (force=true) ya NO se descarta nunca.
      // El /play del ESP32 es BLOQUEANTE (responde cuando termina el audio) y
      // descarta la petición si aún hay audio o si la anterior arrancó hace
      // <500 ms. Con el debounce de v13.10/13.13, el replay de refuerzo del
      // acierto seguía sonando cuando entraba la carta nueva (1300 ms después)
      // -> playInFlight seguía en true -> carta muda -> había que pulsar
      // REPETIR. Ahora: forzadas = suenan siempre, esperando el hueco mínimo
      // de 600 ms que exige el firmware; manuales = conservan el debounce.
      const nowMs = Date.now();
      if (!force && (game.playInFlight || (nowMs - game.playLastMs) < 1500)) return Promise.resolve();
      const gap = Math.max(0, 600 - (nowMs - game.playLastMs));
      game.playLastMs = nowMs + gap;
      game.playInFlight = true;
      const url = '/play?text=' + encodeURIComponent(ch) + (wpm ? '&wpm=' + wpm : '');
      return new Promise(r => setTimeout(r, gap))
        .then(() => fetch(url))
        .catch(() => {})
        .finally(() => { game.playInFlight = false; });
    }
    function buildOptions(correct) {
      // v13.15: 6 opciones (1 correcta + 5 distractores) cuando el set activo lo
      // permite — el grid de 3 columnas queda completo (2 filas de 3) y no deja
      // el hueco que quedaba con 5. Con sets pequeños (Koch 2..5) salen los que
      // haya: no se pueden inventar opciones que no estén en el set activo.
      const chars = selectedChars();
      const opts = [correct];
      const candidates = shuffleArray(chars.filter(c => c !== correct));
      for (let i = 0; i < candidates.length && opts.length < 6; i++) opts.push(candidates[i]);
      return shuffleArray(opts);
    }
    function setOptsEnabled(on) {
      document.querySelectorAll('.game-opt').forEach(b => { b.disabled = !on; });
    }
    function updateHud() {
      const p = document.getElementById('gPos');
      if (p) p.innerText = (game.pos + 1) + '/' + game.rounds;
      const s = document.getElementById('gScore');
      if (s) s.innerText = game.score;
      const st = document.getElementById('gStreak');
      if (st) st.innerText = game.streak;
      const dw = document.getElementById('gDiscWrap');
      if (dw) dw.style.display = (game.escDiscarded > 0) ? '' : 'none';
      const dc = document.getElementById('gDiscarded');
      if (dc) dc.innerText = game.escDiscarded;
    }
    function setGameHint(txt) { const el = document.getElementById('gHint'); if (el) el.innerText = txt; }
    function setEscHint(txt) { const el = document.getElementById('gEscHint'); if (el) el.innerText = txt; }

    // ===================== CONFIG =====================
    function renderGameCfg() {
      game.phase = 'cfg';
      clearTimeout(game.advTimer);
      stopEscPolling();
      game.escActive = false;
      document.getElementById('gameCfg').style.display = 'block';
      document.getElementById('gamePlay').style.display = 'none';
      const ri = document.getElementById('gRoundsInput');
      if (ri && !ri.dataset.bound) {
        ri.dataset.bound = '1';
        ri.addEventListener('change', setGameRoundsCustom);
        ri.addEventListener('blur', setGameRoundsCustom);
      }
      syncCfgButtons();
      renderWeakBox();
    }
    function clampRounds(n) { n = parseInt(n, 10); if (isNaN(n)) return 20; return Math.min(200, Math.max(1, n)); }
    function syncCfgButtons() {
      setBtnActive('gModeLec', game.mode === 'lec');
      setBtnActive('gModeEsc', game.mode === 'esc');
      setBtnActive('gSetLetters', game.set === 'letters');
      setBtnActive('gSetNumbers', game.set === 'numbers');
      setBtnActive('gSetSigns', game.set === 'signs');
      setBtnActive('gSetAdvanced', game.set === 'advanced');
      const ri = document.getElementById('gRoundsInput');
      if (ri) ri.value = game.rounds;
      const k = document.getElementById('gKoch');
      if (k) k.checked = game.koch;
      updateKochInfo();
    }
    // v13.4: transparencia del nivel Koch ("¿por qué estas letras?")
    function updateKochInfo() {
      const el = document.getElementById('gKochInfo');
      if (!el) return;
      if (game.koch) {
        const pool = selectedChars();
        el.innerText = t('g_koch_info').replace('{n}', getKochLevel()).replace('{s}', pool.join(' '));
      } else {
        el.innerText = '';
      }
    }
    function setBtnActive(id, on) {
      const el = document.getElementById(id);
      if (el) el.classList.toggle('btn-active', on);
    }
    function setGameMode(m) { game.mode = m; saveGameCfg(); syncCfgButtons(); }
    function setGameSet(s) { game.set = s; saveGameCfg(); syncCfgButtons(); renderWeakBox(); }
    function setGameRounds(n) { game.rounds = clampRounds(n); saveGameCfg(); syncCfgButtons(); }
    function setGameRoundsCustom() {
      const ri = document.getElementById('gRoundsInput');
      if (!ri) return;
      const v = clampRounds(ri.value);
      ri.value = v; // normaliza lo que escribió (1-200, fallback 20)
      if (v !== game.rounds) { game.rounds = v; saveGameCfg(); syncCfgButtons(); }
    }
    function toggleGameKoch() {
      const k = document.getElementById('gKoch');
      game.koch = k.checked;
      saveGameCfg(); syncCfgButtons(); renderWeakBox();
    }
    function weakBoxHtml() {
      const chars = selectedChars();
      let html = '<div class="weak-title">' + t('g_weak_title') + '</div><div class="weak-grid">';
      chars.forEach(c => {
        const s = game.stats[c] || { v: 0, h: 0, f: 0, r: 0 };
        const st = stateOf(c);
        const cls = st === 'm' ? 'w-m' : (st === 't' ? 'w-t' : 'w-n');
        const acc = s.v > 0 ? Math.round(100 * s.h / s.v) : '–';
        html += '<span class="weak-cell ' + cls + '" title="' + c + ' · ' + s.v + ' · ' + acc + '% · racha ' + s.r + '">' + c + '<i>' + acc + '%</i></span>';
      });
      html += '</div>';
      return html;
    }
    function renderWeakBox() {
      const box = document.getElementById('gameWeakBox');
      if (box) box.innerHTML = weakBoxHtml();
    }
    function gameResetStats() {
      if (confirm(t('g_reset_confirm'))) {
        try { localStorage.removeItem(GAME_STATS_KEY); } catch (e) {}
        // v13.4: reiniciar también el nivel Koch (antes quedaba "clavado"
        // en el nivel de sesiones anteriores y confundía: "¿por qué 12 letras?").
        setKochLevel(5);
        loadGameStats();
        syncCfgButtons();
        renderWeakBox();
      }
    }

    // ===================== INICIO / TEST =====================
    function gameStart() {
      saveGameCfg();
      const chars = selectedChars();
      if (chars.length < 2) { alert(t('g_need_chars')); return; }
      game.score = 0; game.streak = 0; game.bestStreak = 0;
      game.deck = []; game.deckPos = 0; game.deckKey = ''; // v13.8: ciclo fresco
      // v13.3: sin test de ubicación — el
      // set dinámico aprende las letras débiles jugando (peso 3.0/1.5/0.7).
      beginRound();
    }

    // v13.8: MAZO SIN REPETICIONES — ninguna letra se repite
    // hasta cubrir todas las activas. El mazo persiste entre rondas; al agotarse
    // se re-baraja. Si el pool cambia (Koch sube de nivel), el mazo se reconstruye
    // al momento y las letras NUEVAS (menos vistas) van al frente de la cola —
    // la letra recién agregada aparece en la próxima ronda seguro.
    function deckDraw() {
      const pool = selectedChars();
      const key = pool.join('');
      if (!game.deck || game.deckPos >= game.deck.length || game.deckKey !== key) {
        game.deck = shuffleArray(pool.slice()).sort((a, b) => {
          const va = (game.stats[a] && game.stats[a].v) || 0;
          const vb = (game.stats[b] && game.stats[b].v) || 0;
          return va - vb; // sort estable: menos vistas primero, resto barajado
        });
        game.deckKey = key;
        game.deckPos = 0;
      }
      return game.deck[game.deckPos++];
    }
    function beginRound() {
      const chars = selectedChars();
      if (chars.length < 2) { alert(t('g_need_chars')); renderGameCfg(); return; }
      const round = [];
      for (let i = 0; i < game.rounds; i++) round.push(deckDraw());
      game.round = round;
      game.pos = 0;
      renderGamePlay();
    }
    function renderGamePlay() {
      game.phase = 'play';
      document.getElementById('gameCfg').style.display = 'none';
      document.getElementById('gamePlay').style.display = 'block';
      document.getElementById('gameDone').style.display = 'none';
      document.getElementById('gLecView').style.display = (game.mode === 'lec') ? 'block' : 'none';
      document.getElementById('gEscView').style.display = (game.mode === 'esc') ? 'block' : 'none';
      document.getElementById('gTimingWrap').style.display = (game.mode === 'esc') ? '' : 'none';
      updateHud();
      if (game.mode === 'lec') renderLecChar(); else renderEscChar();
    }
    function advanceAfter(ms) {
      clearTimeout(game.advTimer);
      game.advTimer = setTimeout(() => {
        // v13.1: SIN fetch('/set_decoder?state=0') aquí — competía con el
        // state=1 del siguiente carácter (conexiones HTTP paralelas) y podía
        // dejar el decoder APAGADO ("se queda colgado, no reconoce lo tecleado").
        // v13.10: /stop para vaciar reproducciones encoladas antes de avanzar.
        fetch('/stop').catch(() => {});
        if (game.mode === 'esc') stopEscPolling();
        game.pos++;
        if (game.pos >= game.rounds) { finishRound(); return; }
        updateHud();
        if (game.mode === 'lec') renderLecChar(); else renderEscChar();
      }, ms);
    }
    function finishRound() {
      game.phase = 'done';
      if (game.mode === 'esc') {
        stopEscPolling();
        // v13.12: sin cerrojo — pausar el decoder al terminar la ronda (v13.9).
        fetch('/set_decoder?state=0').catch(() => {});
        fetch('/stop').catch(() => {});
      }
      // v13.9: guardar el nivel de la ronda recién jugada (para REPETIRLA sin
      // sumar caracteres nuevos) y mostrar el botón repetir solo en modo Koch.
      game.repeatKochLevel = getKochLevel();
      document.getElementById('gRepeatRoundBtn').style.display = game.koch ? '' : 'none';
      // v13.1: el nivel Koch crece +1 por ronda, con tope en el tamaño REAL
      // del set (sin el recorte del nivel — antes se quedaba clavado).
      if (game.koch) setKochLevel(Math.min(getKochLevel() + 1, selectedChars(true).length));
      document.getElementById('gLecView').style.display = 'none';
      document.getElementById('gEscView').style.display = 'none';
      document.getElementById('gDoneScore').innerText = game.score;
      document.getElementById('gDoneBest').innerText = game.bestStreak;
      // v13.1: matriz de estado por carácter en la pantalla de ronda completa
      document.getElementById('gDoneWeak').innerHTML = weakBoxHtml();
      document.getElementById('gameDone').style.display = 'block';
      saveGameStats();
    }
    function gameNextRound() { beginRound(); }
    // v13.9: repetir la ronda con el MISMO nivel Koch (sin +1). finishRound ya
    // incrementó el nivel; aquí volvemos al guardado y arrancamos ronda fresca.
    // El mazo se reconstruye solo (cambió el pool) y las activas quedan igual.
    function gameRepeatRound() {
      if (game.koch) setKochLevel(game.repeatKochLevel);
      beginRound();
    }
    function gameQuit() {
      clearTimeout(game.advTimer);
      stopEscPolling();
      // v13.12: sin cerrojo — pausar el decoder al salir (v13.9).
      fetch('/set_decoder?state=0').catch(() => {});
      renderGameCfg();
    }

    // ===================== MODO LECTURA =====================
    function renderLecChar() {
      game.escActive = false;
      const ch = game.round[game.pos];
      game.attempts = 0;
      const letter = document.getElementById('gLetter');
      letter.innerText = '?';            // v13.1: oculta = '?' (sin blur)
      letter.classList.remove('crisp');
      setGameHint('');
      const opts = buildOptions(ch);
      const box = document.getElementById('gOpts');
      box.innerHTML = '';
      opts.forEach(o => {
        const b = document.createElement('button');
        b.className = 'game-opt'; b.dataset.ch = o; b.innerText = o;
        b.onclick = () => gameAnswerLec(b, o);
        box.appendChild(b);
      });
      setOptsEnabled(false);
      // v13.14: la carta nueva SIEMPRE suena (force) — playLetter() respeta el
      // gate de 500 ms del firmware sin descartar el sonido de la carta.
      playLetter(ch, null, true);
      setTimeout(() => setOptsEnabled(true), morseDurationMs(morseMap[ch], gameWpm()) + 150);
    }
    function gameAnswerLec(btn, picked) {
      if (game.phase !== 'play') return;
      const target = game.round[game.pos];
      const all = document.querySelectorAll('#gOpts .game-opt');
      if (picked === target) {
        registerHit(target);
        all.forEach(b => { if (b !== btn) b.disabled = true; });
        btn.classList.add('right');
        const letter = document.getElementById('gLetter');
        letter.innerText = target;      // v13.1: revelar la letra real
        letter.classList.add('crisp');
        setGameHint('✅ ' + t('g_right'));
        playLetter(target);
        // v13.14: esperar a que termine el replay de refuerzo antes de pasar de
        // carta. A WPM bajos duraba más de 1300 ms, seguía sonando al entrar la
        // carta nueva y bloqueaba su reproducción (carta muda).
        advanceAfter(Math.max(1300, morseDurationMs(morseMap[target], gameWpm()) + 350));
      } else {
        registerFail(target);
        game.attempts++;
        btn.classList.add('wrong');
        btn.disabled = true;
        const letter = document.getElementById('gLetter');
        if (game.attempts >= 3) {
          const letter = document.getElementById('gLetter');
          letter.innerText = target;    // v13.1: revelar la letra real
          letter.classList.add('crisp');
          all.forEach(b => { if (b.dataset.ch === target) b.classList.add('right'); b.disabled = true; });
          setGameHint(t('g_answer') + ' ' + target + ' (' + morseMap[target] + ')');
          playLetter(target);
          advanceAfter(2200);
        } else {
          const wpm = Math.max(5, Math.round(gameWpm() * 0.7));
          if (game.attempts === 1) setGameHint('🔉 ' + t('g_clue_slow'));
          else setGameHint(t('g_clue_pattern') + ' ' + morseMap[target]);
          playLetter(target, wpm);
        }
      }
    }
    function gameRepeat() {
      if (game.phase !== 'play' || game.mode !== 'lec') return;
      playLetter(game.round[game.pos]);
    }
    function gameGiveUp() {
      if (game.phase !== 'play') return;
      const target = game.round[game.pos];
      registerFail(target);
      const letter = document.getElementById('gLetter');
      letter.innerText = target;        // v13.1: revelar la letra real
      letter.classList.add('crisp');
      document.querySelectorAll('#gOpts .game-opt').forEach(b => {
        if (b.dataset.ch === target) b.classList.add('right');
        b.disabled = true;
      });
      setGameHint(t('g_answer') + ' ' + target + ' (' + morseMap[target] + ')');
      playLetter(target);
      advanceAfter(2200);
    }

    // ===================== MODO ESCRITURA =====================
    function renderEscChar() {
      const ch = game.round[game.pos];
      game.attempts = 0; game.copy = false; game.escMirror = ''; game.escTotal = 0; game.escActive = false;
      game.escDiscarded = 0; game.escLastChar = 0;
      const letter = document.getElementById('gEscLetter');
      letter.innerText = ch;
      letter.classList.remove('crisp-ok', 'crisp-bad');
      document.getElementById('gEscSub').innerText = t('g_key_hint');
      setEscHint('');
      // v13.12: sin cerrojo — encender decoder + limpiar buffer en un request
      // (v13.9 lo hacía en dos: state=1 y luego /clear_decoded). El polling
      // arranca directo; el reset evita falsos errores de buffer acumulado.
      fetch('/set_decoder?state=1&reset=1').catch(() => {});
      game.escActive = true;
      startEscPolling();
    }
    function startEscPolling() {
      stopEscPolling();
      game.escBusy = false;
      game.escTimer = setInterval(() => {
        if (game.escBusy) return;
        game.escBusy = true;
        // v13.1: timeout anti-cuelgue — si el ESP32 está ocupado (audio) o la
        // conexión se cae, el fetch no puede colgar el polling para siempre.
        const ctrl = new AbortController();
        const pollTo = setTimeout(() => ctrl.abort(), 900);
        fetch('/get_decoded?since=' + game.escTotal, { signal: ctrl.signal })
          .then(r => r.json())
          .then(d => {
            if (d.n !== undefined && d.n < game.escTotal) { game.escTotal = 0; game.escMirror = ''; }
            let fresh = '';
            if (d.full) {
              // buffer completo: dedupe por prefijo contra el espejo
              const buf = d.text || '';
              if (buf.length >= game.escMirror.length && buf.startsWith(game.escMirror)) {
                fresh = buf.substring(game.escMirror.length);
              } else {
                fresh = buf; // rotación inusual: procesar lo recibido
              }
              game.escMirror = buf;
            } else {
              // incremental (since>0): el delta ES lo nuevo por definición
              fresh = d.text || '';
              game.escMirror += fresh;
            }
            const chars = fresh.replace(/[^A-Z0-9.,?/=]/g, '').split('');
            // v13.10: gate anti-ráfaga — 1 carácter por tick (el resto se
            // descarta y cuenta). Un buffer viejo ya no puede generar 3 fallos
            // seguidos ni disparar la fase copy (y sus sonidos).
            let processed = 0;
            for (let i = 0; i < chars.length; i++) {
              if (processed >= 1) { game.escDiscarded += (chars.length - i); updateHud(); break; }
              if (game.escActive && chars[i]) { onEscChar(chars[i]); processed++; }
            }
            if (d.n !== undefined) game.escTotal = d.n;
            if (d.timing !== undefined) {
              const el = document.getElementById('gTiming');
              if (el) {
                el.innerText = d.timing;
                el.style.color = d.timing >= 80 ? '#2ecc71' : (d.timing >= 50 ? '#f1c40f' : '#e74c3c');
              }
            }
          }).catch(() => {})
          .finally(() => { clearTimeout(pollTo); game.escBusy = false; });
      }, 200);
    }
    function stopEscPolling() {
      if (game.escTimer) { clearInterval(game.escTimer); game.escTimer = null; }
    }
    function onEscChar(c) {
      if (game.phase !== 'play' || !game.escActive || game.mode !== 'esc') return;
      // v13.10: debounce por carácter (>=150 ms). A 30 WPM un carácter completo
      // lleva >400 ms; menos que eso es basura acumulada, no input real.
      const nowMs = Date.now();
      if (nowMs - game.escLastChar < 150) { game.escDiscarded++; updateHud(); return; }
      game.escLastChar = nowMs;
      const target = game.round[game.pos];
      if (c === '?' && target !== '?') { setEscHint(t('g_esc_invalid')); return; }
      const letter = document.getElementById('gEscLetter');
      if (game.copy) {
        if (c === target) {
          registerHit(target);
          game.escActive = false;
          letter.classList.add('crisp-ok');
          document.getElementById('gEscSub').innerText = '✅ ' + t('g_copy_done');
          advanceAfter(1200);
        } else {
          setEscHint(t('g_copy_wrong') + ' ' + c);
          playLetter(target);
        }
        return;
      }
      if (c === target) {
        registerHit(target);
        game.escActive = false;
        letter.classList.add('crisp-ok');
        document.getElementById('gEscSub').innerText = '✅ ' + t('g_right');
        advanceAfter(1300);
      } else {
        registerFail(target);
        game.attempts++;
        letter.classList.add('crisp-bad');
        document.getElementById('gEscSub').innerText = '❌ ' + t('g_esc_was') + ' ' + c;
        if (game.attempts >= 3) {
          enterCopyPhase(target);
        } else {
          if (game.attempts === 2) setEscHint(t('g_esc_clue') + ' ' + morseMap[target]);
          clearTimeout(game.advTimer);
          game.advTimer = setTimeout(() => {
            letter.classList.remove('crisp-bad');
            document.getElementById('gEscSub').innerText = t('g_key_hint');
          }, 900);
        }
      }
    }
    function enterCopyPhase(target) {
      game.copy = true;
      document.getElementById('gEscSub').innerText = t('g_copy') + ' ' + morseMap[target];
      playLetter(target);
    }
    function gameDontKnow() {
      if (game.phase !== 'play') return;
      const target = game.round[game.pos];
      registerFail(target);
      enterCopyPhase(target);
    }

    // ===================== TECLADO 1-5 (escritorio) =====================
    document.addEventListener('keydown', (ev) => {
      if (document.activeElement && ['INPUT', 'TEXTAREA', 'SELECT'].indexOf(document.activeElement.tagName) >= 0) return;
      const idx = ['1', '2', '3', '4', '5'].indexOf(ev.key);
      if (idx < 0) return;
      if (game.phase === 'play' && game.mode === 'lec') {
        const btns = document.querySelectorAll('#gOpts .game-opt');
        if (btns[idx] && !btns[idx].disabled) gameAnswerLec(btns[idx], btns[idx].dataset.ch);
      }
    });

    window.onload = () => {
      loadGameStats();
      loadGameCfg();
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
    // Al cerrar/abandonar la página, pausar el decoder y liberar el cerrojo
    // (v13.11: con el token correcto; antes el release sin claim no liberaba
    // nada y el state=0 sin claim podía apagar el decoder de OTRA pestaña).
    window.addEventListener('beforeunload', () => {
      // v13.12: sin cerrojo — pausar el decoder al cerrar/recargar (v13.9).
      if (navigator.sendBeacon) {
        navigator.sendBeacon('/set_decoder?state=0');
      } else {
        fetch('/set_decoder?state=0');
      }
    });

    // v13.10: si la pestaña queda oculta a mitad de una carta de escritura,
    // pausar el juego localmente (los timers de fondo no procesan input y no
    // disparan sonidos). Al volver, se re-sincroniza con el decoder.
    document.addEventListener('visibilitychange', () => {
      if (document.hidden) {
        stopEscPolling();
        game.escActive = false;
      } else if (game.phase === 'play' && game.mode === 'esc' && !game.escActive) {
        // v13.12: al volver a la pestaña con carta en curso, re-encender y
        // re-sincronizar (sin cerrojo).
        fetch('/set_decoder?state=1&reset=1').catch(() => {});
        game.escActive = true;
        startEscPolling();
      }
    });
  </script>
</body>
</html>
)rawliteral";

#endif
