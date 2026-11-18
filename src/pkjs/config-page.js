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
  ['BatteryIndicator', 'Show battery bar'],
  ['QuietTimeIndicator', 'Show quiet-time indicator (right)'],
  ['DisconnectIndicator', 'Show disconnection indicator (left)'],
  ['OutlinedArcs', 'Outlined arc borders'],
  ['ShowIndicatorText', 'Show indicator text'],
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

function build(settings) {
  var palette = [];
  if (cfg.isColorWatch) {
    cfg.colorLayout.forEach(function(r) { r.forEach(function(c) { if (c) palette.push(c); }); });
  } else {
    palette = ['000000', 'FFFFFF'];
  }

  var selects = ['WeatherUpdateInterval'];
  var quadHtml = QUADS.map(function(q) {
    selects.push(q[0] + '_Option');
    return '<div class="quad"><div class="t">' + q[1] + '</div>' +
      '<div class="row"><label for="' + q[0] + '_Option">Data</label>' +
      '<select id="' + q[0] + '_Option">' + optionTags(cfg.options) + '</select></div>' +
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
    palette: palette
  };
  var json = JSON.stringify(data).replace(/</g, '\\u003c');

  var body =
    '<h1>ARCs</h1>' +
    '<div class="card"><h2>Theme</h2>' + toggleRow('DarkTheme') + '</div>' +
    '<div class="card"><h2>Standalone Battery Indicator</h2>' +
      '<div class="hint">Show a standalone battery indicator beneath the date</div>' +
      toggleRow('BatteryIndicator') + '</div>' +
    '<div class="card"><h2>Statuses</h2>' +
      '<div class="hint">Show status indicators on the watchface edges</div>' +
      toggleRow('QuietTimeIndicator') + toggleRow('DisconnectIndicator') + '</div>' +
    '<div class="card"><h2>Style</h2>' + toggleRow('OutlinedArcs') + '</div>' +
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
    '<div class="bar"><button id="save">Save</button></div>';

  return '<!DOCTYPE html><html><head><meta charset="utf-8">' +
    '<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">' +
    '<title>ARCs</title><style>' + CSS + '</style></head><body>' + body +
    '<script>window.__DATA__=' + json + ';' + SCRIPT + '</script></body></html>';
}

module.exports = { build: build };
