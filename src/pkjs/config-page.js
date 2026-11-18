// Custom configuration page (HTML + CSS + JS) served as a data: URI.
var cfg = require('./config');

var CSS = [
  ':root{--bg:#0f1115;--card:#1a1d24;--text:#eceff4;--muted:#8b93a1;--accent:#ff7a3d;--line:#2a2e38;}',
  'body.light{--bg:#f2f3f5;--card:#fff;--text:#1b1d22;--muted:#6b7280;--line:#e2e4e9;}',
  '*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}',
  'body{margin:0;padding:16px 16px 96px;background:var(--bg);color:var(--text);',
  'font:16px/1.4 -apple-system,Roboto,Helvetica,Arial,sans-serif}',
  'h1{font-size:28px;margin:8px 0 16px;letter-spacing:1px}',
  '.card{background:var(--card);border-radius:14px;padding:14px 16px;margin-bottom:14px;border:1px solid var(--line)}',
  '.card h2{margin:0 0 4px;font-size:13px;text-transform:uppercase;letter-spacing:1.2px;color:var(--accent)}',
  '.hint{color:var(--muted);font-size:13px;margin:2px 0 8px}',
  '.row{display:flex;align-items:center;justify-content:space-between;gap:12px;padding:10px 0;border-top:1px solid var(--line)}',
  '.row:first-of-type{border-top:0}',
  '.row label{flex:1}',
  'select,input[type=number]{background:var(--bg);color:var(--text);border:1px solid var(--line);border-radius:8px;',
  'padding:8px 10px;font-size:16px;max-width:55%}',
  'input[type=number]{width:120px;text-align:right}',
  'input[type=range]{flex:1;accent-color:var(--accent)}',
  '.val{min-width:28px;text-align:right;color:var(--muted)}',
  '.sw{position:relative;width:48px;height:28px;flex:none}',
  '.sw input{opacity:0;width:100%;height:100%;margin:0;position:absolute;z-index:1}',
  '.sw i{position:absolute;inset:0;background:var(--line);border-radius:14px;transition:.15s}',
  '.sw i:after{content:"";position:absolute;width:22px;height:22px;left:3px;top:3px;background:#fff;border-radius:50%;transition:.15s}',
  '.sw input:checked+i{background:var(--accent)}',
  '.sw input:checked+i:after{transform:translateX(20px)}',
  '.quad{border-top:1px solid var(--line);padding:12px 0 4px}',
  '.quad:first-of-type{border-top:0}',
  '.quad .t{font-weight:600;margin-bottom:8px}',
  '.grid{display:flex;flex-wrap:wrap;gap:6px;margin-top:10px}',
  '.c{width:30px;height:30px;border-radius:8px;border:2px solid transparent;box-shadow:inset 0 0 0 1px rgba(128,128,128,.4)}',
  '.c.on{border-color:var(--text);transform:scale(1.12)}',
  '.pick{display:flex;align-items:center;gap:6px;background:var(--bg);color:var(--text);border:1px solid var(--line);border-radius:8px;padding:8px 10px;font-size:16px;max-width:60%}',
  '.pick:after{content:"\\203A";transform:rotate(90deg);color:var(--muted);font-size:20px;line-height:1}',
  '.ov{position:fixed;inset:0;background:rgba(0,0,0,.5);opacity:0;pointer-events:none;transition:opacity .2s;z-index:10}',
  '.ov.open{opacity:1;pointer-events:auto}',
  '.sheet{position:fixed;left:0;right:0;bottom:0;max-height:80vh;display:flex;flex-direction:column;background:var(--card);',
  'border-radius:18px 18px 0 0;transform:translateY(100%);transition:transform .25s ease;z-index:11}',
  '.ov.open .sheet{transform:translateY(0)}',
  '.sheet .grab{width:40px;height:4px;border-radius:2px;background:var(--line);margin:10px auto 6px;flex:none}',
  '.sheet .st{display:flex;align-items:center;justify-content:space-between;padding:4px 12px 10px 20px;font-weight:600;font-size:17px;flex:none;border-bottom:1px solid var(--line)}',
  '.sheet .x{width:36px;height:36px;border:0;border-radius:50%;background:var(--bg);color:var(--muted);font-size:16px;line-height:1}',
  '.sheet .x:active{opacity:.7}',
  '.sheet .sb{overflow-y:auto;overscroll-behavior:contain;-webkit-overflow-scrolling:touch;padding:0 8px 24px}',
  '.sec{padding:16px 12px 6px;font-size:12px;text-transform:uppercase;letter-spacing:1.2px;color:var(--accent)}',
  '.opt{display:flex;justify-content:space-between;align-items:center;padding:13px 12px;border-radius:10px}',
  '.opt:active{background:var(--bg)}',
  '.opt .ot{display:flex;flex-direction:column;gap:2px}',
  '.opt small{color:var(--muted);font-size:12px}',
  '.opt small.why{color:#e5534b}',
  '.opt.off{opacity:.6}',
  '.opt.off span{text-decoration:none}',
  '.opt.on{color:var(--accent);font-weight:600}',
  '.opt.on:after{content:"\\2713"}',
  'body.lock{overflow:hidden}',
  '.bar{position:fixed;left:0;right:0;bottom:0;padding:12px 16px;background:var(--bg);border-top:1px solid var(--line)}',
  '.bar button{width:100%;padding:14px;border:0;border-radius:12px;background:var(--accent);color:#fff;font-size:17px;font-weight:600}',
  '.bar button:active{opacity:.8}'
].join('\n');

var SCRIPT = [
  'var D=window.__DATA__,S=D.settings,$=function(i){return document.getElementById(i)};',
  'function apply(){document.body.className=S.DarkTheme?"":"light"}',
  'function toggle(k){var e=$(k);e.checked=!!S[k];e.onchange=function(){S[k]=e.checked;apply()}}',
  'function select(k){var e=$(k);e.value=String(S[k]);e.onchange=function(){S[k]=parseInt(e.value,10)}}',
  'function num(k){var e=$(k);e.value=S[k];e.oninput=function(){S[k]=e.value}}',
  'D.toggles.forEach(toggle);D.selects.forEach(select);D.nums.forEach(num);',
  'function label(v){var l="";D.sections.forEach(function(s){s.options.forEach(function(o){if(o.value===v)l=o.label})});return l||String(v)}',
  'var ov=$("ov"),sb=$("sheetBody"),cur=null;',
  'function closeSheet(){ov.className="ov";document.body.classList.remove("lock");cur=null}',
  'function openSheet(i){cur=i;var q=D.quads[i],key=q+"_Option";$("sheetTitle").textContent=D.quadNames[i];sb.innerHTML="";',
  ' D.sections.forEach(function(s){var h=document.createElement("div");h.className="sec";h.textContent=s.title;sb.appendChild(h);',
  '  s.options.forEach(function(o){var st=D.status[o.value]||{},off=st.enabled===false;',
  '   var r=document.createElement("div");r.className="opt"+(S[key]===o.value?" on":"")+(off?" off":"");',
  '   var t=document.createElement("div");t.className="ot";var l=document.createElement("span");l.textContent=o.label;t.appendChild(l);',
  '   if(st.detail){var d=document.createElement("small");d.textContent=st.detail;t.appendChild(d)}',
  '   if(off&&st.reason){var w=document.createElement("small");w.className="why";w.textContent=st.reason;t.appendChild(w)}',
  '   r.appendChild(t);if(off)r.setAttribute("aria-disabled","true");',
  '   r.onclick=function(){if(off)return;S[key]=o.value;$(q+"_pick").textContent=o.label;closeSheet()};sb.appendChild(r)})});',
  ' sb.scrollTop=0;document.body.classList.add("lock");ov.className="ov open"}',
  '$("sheetClose").onclick=closeSheet;',
  'ov.onclick=function(e){if(e.target===ov)closeSheet()};',
  'document.addEventListener("keydown",function(e){if(e.key==="Escape")closeSheet()});',
  'D.quads.forEach(function(q,i){S[q+"_Option"]=parseInt(S[q+"_Option"],10);$(q+"_pick").textContent=label(S[q+"_Option"]);$(q+"_pick").onclick=function(){openSheet(i)}});',
  'var w=$("IndicatorWidth"),wv=$("IndicatorWidthVal");w.value=S.IndicatorWidth;wv.textContent=w.value;',
  'w.oninput=function(){S.IndicatorWidth=parseInt(w.value,10);wv.textContent=w.value};',
  'D.quads.forEach(function(q){var g=$(q+"_grid"),key=q+"_Colour";',
  ' D.palette.forEach(function(c){var b=document.createElement("div");b.className="c";',
  '  b.style.background="#"+c;b.onclick=function(){S[key]=c;paint()};b.dataset.c=c;g.appendChild(b)});});',
  'function paint(){D.quads.forEach(function(q){var cur=String(S[q+"_Colour"]).toLowerCase();',
  ' Array.prototype.forEach.call($(q+"_grid").children,function(b){',
  '  b.className="c"+(b.dataset.c.toLowerCase()===cur?" on":"")})})}',
  'paint();apply();',
  '$("save").onclick=function(){document.location="pebblejs://close#"+encodeURIComponent(JSON.stringify(S))};'
].join('\n');

function esc(s) {
  return String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;');
}

var TOGGLES = [
  ['DarkTheme', 'Dark Theme'],
  ['QuietTimeIndicator', 'Show quiet-time indicator (right)'],
  ['DisconnectIndicator', 'Show disconnection indicator (left)'],
  ['OutlinedArcs', 'Outlined arc borders'],
  ['ShowIndicatorText', 'Show indicator text'],
  ['AnimateIndicators', 'Animate indicators'],
  ['WeatherUseCelsius', 'Use Celsius']
];
var QUADS = [
  ['TopLeft', 'Top Left'], ['TopRight', 'Top Right'],
  ['BottomLeft', 'Bottom Left'], ['BottomRight', 'Bottom Right']
];
var NUMS = [
  ['StepGoal', 'Steps Goal'],
  ['HeartRateLower', 'Heart Rate lower bound'],
  ['HeartRateUpper', 'Heart Rate upper bound'],
  ['CalorieGoal', 'Calorie Goal'],
  ['DistanceGoal', 'Distance Goal (metres)'],
  ['TemperatureLower', 'Temperature lower bound'],
  ['TemperatureUpper', 'Temperature upper bound']
];

function toggleRow(id) {
  var label = TOGGLES.filter(function(t) { return t[0] === id; })[0][1];
  return '<div class="row"><label for="' + id + '">' + label + '</label>' +
    '<span class="sw"><input type="checkbox" id="' + id + '"><i></i></span></div>';
}

function optionTags(opts) {
  return opts.map(function(o) {
    return '<option value="' + o.value + '">' + esc(o.label) + '</option>';
  }).join('');
}

function build(settings, pluginStatus) {
  var palette = [];
  if (cfg.isColorWatch) {
    cfg.colorLayout.forEach(function(r) { r.forEach(function(c) { if (c) palette.push(c); }); });
  } else {
    palette = ['000000', 'FFFFFF'];
  }

  var selects = ['WeatherUpdateInterval'];
  var quadHtml = QUADS.map(function(q) {
    return '<div class="quad"><div class="t">' + q[1] + '</div>' +
      '<div class="row"><label>Data</label>' +
      '<button type="button" class="pick" id="' + q[0] + '_pick"></button></div>' +
      '<div class="grid" id="' + q[0] + '_grid"></div></div>';
  }).join('');

  var numHtml = NUMS.map(function(n) {
    return '<div class="row"><label for="' + n[0] + '">' + n[1] + '</label>' +
      '<input type="number" inputmode="numeric" id="' + n[0] + '"></div>';
  }).join('');

  var data = {
    settings: settings,
    toggles: TOGGLES.map(function(t) { return t[0]; }),
    selects: selects,
    nums: NUMS.map(function(n) { return n[0]; }),
    quads: QUADS.map(function(q) { return q[0]; }),
    palette: palette,
    status: pluginStatus || {},
    quadNames: QUADS.map(function(q) { return q[1]; }),
    sections: [
      { title: 'Indicators', options: cfg.indicatorOptions },
      { title: 'Plugin Data', options: cfg.pluginOptions }
    ]
  };
  var json = JSON.stringify(data).replace(/</g, '\\u003c');

  var body =
    '<h1>ARCs</h1>' +
    '<div class="card"><h2>Theme</h2>' + toggleRow('DarkTheme') + '</div>' +
    '<div class="card"><h2>Statuses</h2>' +
      '<div class="hint">Show status indicators on the watchface edges</div>' +
      toggleRow('QuietTimeIndicator') + toggleRow('DisconnectIndicator') + '</div>' +
    '<div class="card"><h2>Style</h2>' + toggleRow('OutlinedArcs') + toggleRow('AnimateIndicators') + '</div>' +
    '<div class="card"><h2>Indicators</h2>' + toggleRow('ShowIndicatorText') +
      '<div class="row"><label for="IndicatorWidth">Indicator Width</label>' +
      '<input type="range" id="IndicatorWidth" min="1" max="20" step="1">' +
      '<span class="val" id="IndicatorWidthVal"></span></div>' + quadHtml + '</div>' +
    '<div class="card"><h2>Configuration</h2>' +
      '<div class="hint">For the relevant indicators above, set goals and bounds here</div>' + numHtml + '</div>' +
    '<div class="card"><h2>Weather</h2>' +
      '<div class="hint">Uses GPS to fetch current weather from Open-Meteo. Select "Temperature" or "Weather" in the Indicators section to display weather data.</div>' +
      '<div class="row"><label for="WeatherUpdateInterval">Update interval</label>' +
      '<select id="WeatherUpdateInterval">' +
      optionTags([{ label: '15 minutes', value: 15 }, { label: '30 minutes', value: 30 }, { label: '60 minutes', value: 60 }]) +
      '</select></div>' + toggleRow('WeatherUseCelsius') +
      '<div class="hint">Weather condition abbreviations<br>' + cfg.abbreviations.map(esc).join('<br>') + '</div></div>' +
    '<div class="ov" id="ov"><div class="sheet"><div class="grab"></div><div class="st"><span id="sheetTitle"></span><button type="button" class="x" id="sheetClose" aria-label="Close">\u2715</button></div><div class="sb" id="sheetBody"></div></div></div>' +
    '<div class="bar"><button id="save">Save</button></div>';

  return '<!DOCTYPE html><html><head><meta charset="utf-8">' +
    '<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">' +
    '<title>ARCs</title><style>' + CSS + '</style></head><body>' + body +
    '<script>window.__DATA__=' + json + ';' + SCRIPT + '</script></body></html>';
}

module.exports = { build: build };
