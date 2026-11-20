var cfg = require('./config');
var configPage = require('./config-page');

var STORAGE_KEY = 'arcs-settings';

function loadSettings() {
  var saved = {};
  try {
    // Fall back to settings previously saved by Clay so users keep their config
    saved = JSON.parse(localStorage.getItem(STORAGE_KEY) || localStorage.getItem('clay-settings') || '{}');
  } catch (e) {}
  var s = {};
  for (var k in cfg.defaults) {
    s[k] = (saved[k] !== undefined && saved[k] !== null) ? saved[k] : cfg.defaults[k];
  }
  // Clay stored colours as ints/hex strings; normalise to 6-digit hex
  ['TopLeft', 'TopRight', 'BottomLeft', 'BottomRight'].forEach(function(q) {
    var c = s[q + '_Colour'];
    if (typeof c === 'number') c = ('000000' + c.toString(16)).slice(-6);
    s[q + '_Colour'] = String(c).replace('#', '');
    var opt = parseInt(s[q + '_Option'], 10);
    var b = cfg.indicatorBounds[String(opt)] || { min: 0, max: 100 };
    if (saved[q + '_Min'] === undefined || saved[q + '_Min'] === null) s[q + '_Min'] = b.min;
    if (saved[q + '_Max'] === undefined || saved[q + '_Max'] === null) s[q + '_Max'] = b.max;
  });
  return s;
}

function toWire(s) {
  var m = {};
  ['DarkTheme', 'QuietTimeIndicator', 'DisconnectIndicator',
   'OutlinedArcs', 'ShowIndicatorText', 'WeatherUseCelsius', 'AnimateIndicators'].forEach(function(k) {
    m[k] = s[k] ? 1 : 0;
  });
  ['IndicatorWidth', 'WeatherUpdateInterval', 'TopLeft_Option', 'TopRight_Option',
   'BottomLeft_Option', 'BottomRight_Option',
   'TopLeft_Min', 'TopLeft_Max', 'TopRight_Min', 'TopRight_Max',
   'BottomLeft_Min', 'BottomLeft_Max', 'BottomRight_Min', 'BottomRight_Max'].forEach(function(k) {
    m[k] = parseInt(s[k], 10);
  });
  ['TopLeft_Colour', 'TopRight_Colour', 'BottomLeft_Colour', 'BottomRight_Colour'].forEach(function(k) {
    m[k] = parseInt(s[k], 16);
  });
  return m;
}

// ---------------------------------------------------------------------------
// Plugin data sources (Pebble Plugins preview, mobile app 2.0.0+)
// ---------------------------------------------------------------------------

var comps = require('./complications');

var QUADRANT_KEYS = ['TopLeft', 'TopRight', 'BottomLeft', 'BottomRight'];
var OPTION_PHONE_BATTERY = 11;
var OPTION_COMPLICATION_BASE = 11;   // slot n has option value 11 + n
var SLOT_COUNT = 5;
var phoneBatterySub = null;
var slotSubs = {};
var lastSent = {};

// Permission a complication's source category needs (others need none)
var CATEGORY_PERMISSION = { weather: 'Location', calendar: 'Calendar', music: 'ReadMusic' };

function pluginsSupported() {
  return typeof Pebble.subscribeToSource === 'function';
}

function sendPhoneBattery(value) {
  Pebble.sendAppMessage({ PhoneBattery: value },
    function() { console.log('Phone battery sent: ' + value); },
    function(e) { console.log('Phone battery send failed: ' + JSON.stringify(e)); });
}

// Send a slot's display value, but only when it differs from the last one sent
function emitSlot(n, display) {
  var key = display.percent + '|' + display.label;
  if (lastSent[n] === key) return;
  lastSent[n] = key;
  var msg = {};
  msg['Complication' + n + '_Percent'] = display.percent;
  msg['Complication' + n + '_Label'] = display.label;
  Pebble.sendAppMessage(msg,
    function() { console.log('Complication ' + n + ' sent: ' + key); },
    function(e) { delete lastSent[n]; console.log('Complication ' + n + ' send failed: ' + JSON.stringify(e)); });
}

function slotEntry(update, n) {
  var instances = (update && update.instances) || [];
  for (var i = 0; i < instances.length; i++) {
    if (String(instances[i].instanceId) === String(n)) {
      var p = instances[i].properties || {};
      var text = function(x) { return p[x] && p[x].shortText && p[x].shortText.text; };
      var idx = p.index && p.index.numericValue ? p.index.numericValue.value : 0;
      if (!text('category') || !text('item') || !text('property')) return null;
      return { category: text('category'), item: text('item'), property: text('property'), index: idx };
    }
  }
  return null;
}

function startSlot(n) {
  if (slotSubs[n]) return;
  var inner = null;
  var innerKey = null;
  function stopInner() { if (inner) { inner.unsubscribe(); inner = null; } innerKey = null; }

  var outer = Pebble.subscribeToSource({
    category: 'preferences',
    item: 'complications',
    onData: function(update) {
      var e = slotEntry(update, n);
      if (!e) { stopInner(); emitSlot(n, comps.UNSUPPORTED); return; }
      var key = [e.category, e.item, e.property, e.index].join('/');
      if (key === innerKey) return;
      stopInner();
      innerKey = key;
      inner = Pebble.subscribeToSource({
        category: e.category,
        item: e.item,
        properties: [e.property],
        onData: function(u) {
          var inst = u && u.instances && u.instances[e.index];
          emitSlot(n, comps.toDisplay(inst && inst.properties && inst.properties[e.property]));
        },
        onError: function(err) {
          console.log('Complication ' + n + ' error: ' + (err && err.code));
          emitSlot(n, err && err.code === 'PERMISSION_DENIED' ? comps.NO_PERMISSION : comps.UNSUPPORTED);
        }
      });
    },
    onError: function() { emitSlot(n, comps.UNSUPPORTED); }
  });
  slotSubs[n] = { stop: function() { stopInner(); outer.unsubscribe(); delete lastSent[n]; } };
}

function stopSlot(n) {
  if (slotSubs[n]) { slotSubs[n].stop(); delete slotSubs[n]; }
}

function syncPluginSubscriptions(settings) {
  var used = {};
  QUADRANT_KEYS.forEach(function(q) { used[parseInt(settings[q + '_Option'], 10)] = true; });

  if (!pluginsSupported()) {
    console.log('Plugins not supported by this Pebble app');
    if (used[OPTION_PHONE_BATTERY]) sendPhoneBattery(-1);
    for (var s = 1; s <= SLOT_COUNT; s++) {
      if (used[OPTION_COMPLICATION_BASE + s]) emitSlot(s, comps.UNSUPPORTED);
    }
    return;
  }

  if (used[OPTION_PHONE_BATTERY] && !phoneBatterySub) {
    phoneBatterySub = Pebble.subscribeToSource({
      category: 'phone',
      item: 'phone_state',
      properties: ['battery_level'],
      onData: function(update) {
        var inst = update && update.instances && update.instances[0];
        var prop = inst && inst.properties && inst.properties.battery_level;
        var level = -1;
        if (prop && prop.numericValue && typeof prop.numericValue.value === 'number') {
          level = Math.round(prop.numericValue.value);
        } else if (prop && prop.shortText && prop.shortText.text) {
          level = parseInt(prop.shortText.text, 10);
          if (isNaN(level)) level = -1;
        }
        sendPhoneBattery(level);
      },
      onError: function(err) {
        console.log('Phone battery error: ' + (err && err.code) + ' ' + (err && err.message));
        sendPhoneBattery(-1);
      }
    });
  } else if (!used[OPTION_PHONE_BATTERY] && phoneBatterySub) {
    phoneBatterySub.unsubscribe();
    phoneBatterySub = null;
  }

  for (var n = 1; n <= SLOT_COUNT; n++) {
    if (used[OPTION_COMPLICATION_BASE + n]) startSlot(n); else stopSlot(n);
  }
}

// ---------------------------------------------------------------------------
// Config page: which plugin options can be selected right now?
// Result: { "<optionValue>": { enabled, reason, detail } }
// ---------------------------------------------------------------------------

function readComplicationEntries(cb) {
  var done = false;
  var sub = null;
  function finish(entries) {
    if (done) return;
    done = true;
    try { if (sub) sub.unsubscribe(); } catch (e) {}
    cb(entries);
  }
  setTimeout(function() { finish(null); }, 2000);
  try {
    sub = Pebble.subscribeToSource({
      category: 'preferences',
      item: 'complications',
      onData: function(update) {
        var entries = {};
        for (var n = 1; n <= SLOT_COUNT; n++) entries[n] = slotEntry(update, n);
        finish(entries);
      },
      onError: function() { finish(null); }
    });
  } catch (e) { finish(null); }
}

function queryPermission(name) {
  if (!name || !Pebble.permissions || typeof Pebble.permissions.query !== 'function') {
    return Promise.resolve('granted');
  }
  return Promise.resolve(Pebble.permissions.query(name)).catch(function() { return 'granted'; });
}

function getPluginStatus(cb) {
  var status = {};
  if (!pluginsSupported()) {
    var reason = "Plugins aren't supported by your Pebble app (needs version 2.0.0 or later).";
    status[OPTION_PHONE_BATTERY] = { enabled: false, reason: reason };
    for (var s = 1; s <= SLOT_COUNT; s++) status[OPTION_COMPLICATION_BASE + s] = { enabled: false, reason: reason };
    cb(status);
    return;
  }
  readComplicationEntries(function(entries) {
    if (!entries) { cb(status); return; }   // couldn't check: leave everything enabled
    var pending = [];
    var slots = [];
    for (var n = 1; n <= SLOT_COUNT; n++) {
      (function(n) {
        var e = entries[n];
        var opt = OPTION_COMPLICATION_BASE + n;
        if (!e) {
          status[opt] = { enabled: false, reason: 'Slot ' + n + ' has no reading. Set one in the Pebble app under Settings \u2192 Plugins \u2192 Complications.' };
          return;
        }
        var detail = e.category + ' \u00b7 ' + e.property;
        var perm = CATEGORY_PERMISSION[e.category];
        status[opt] = { enabled: true, detail: detail };
        pending.push(queryPermission(perm).then(function(state) {
          if (state === 'denied') {
            status[opt] = { enabled: false, detail: detail,
              reason: perm + ' permission is denied for this watchface. Allow it in the Pebble app to use this reading.' };
          }
        }));
      })(n);
    }
    Promise.all(pending).then(function() { cb(status); }, function() { cb(status); });
  });
}

Pebble.addEventListener('ready', function() {
  syncPluginSubscriptions(loadSettings());
});

Pebble.addEventListener('showConfiguration', function() {
  getPluginStatus(function(status) {
    var html = configPage.build(loadSettings(), status);
    Pebble.openURL('data:text/html;charset=utf-8,' + encodeURIComponent(html));
  });
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e || !e.response) return;
  var settings;
  try { settings = JSON.parse(decodeURIComponent(e.response)); } catch (err) {
    console.log('Bad config response: ' + err.message);
    return;
  }
  localStorage.setItem(STORAGE_KEY, JSON.stringify(settings));
  Pebble.sendAppMessage(toWire(loadSettings()),
    function() { console.log('Config sent'); },
    function(err) { console.log('Config send failed: ' + JSON.stringify(err)); });
  syncPluginSubscriptions(loadSettings());
});

// ---------------------------------------------------------------------------
// XHR helper
// ---------------------------------------------------------------------------

function xhrRequest(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
}

// ---------------------------------------------------------------------------
// WMO weather code → condition index (0=CLR, 1=CLOU, 2=OVER, 3=FOG,
//                                     4=DRZZ, 5=RAIN, 6=SNOW, 7=STRM)
// ---------------------------------------------------------------------------

function weatherCodeToCondition(code) {
  console.log(`Weather code: ${code}`);
  if (code === 0)  return 0;  // CLR  - Clear
  if (code === 1)  return 0;  // CLR  - Mainly Clear
  if (code === 2)  return 1;  // CLOU - Cloudy
  if (code === 3)  return 2;  // OVER - Overcast
  if (code <= 48)  return 3;  // FOG  - Fog
  if (code <= 57)  return 4;  // DRZZ - Drizzle / Freezing Drizzle
  if (code <= 67)  return 5;  // RAIN - Rain / Freezing Rain
  if (code <= 77)  return 6;  // SNOW - Snow
  if (code <= 82)  return 5;  // RAIN - Showers
  if (code <= 86)  return 6;  // SNOW - Snow Showers
  return 7;                   // STRM - Thunderstorm
}

// ---------------------------------------------------------------------------
// Weather fetch
// ---------------------------------------------------------------------------

function locationError(err) {
  console.log('Location error: ' + err.message);
}

function locationSuccess(pos) {
  var url = 'https://api.open-meteo.com/v1/forecast' +
    '?latitude=' + pos.coords.latitude +
    '&longitude=' + pos.coords.longitude +
    '&current=temperature_2m,weather_code';

  xhrRequest(url, 'GET', function(responseText) {
    console.log('Weather response: ' + responseText);
    var json = JSON.parse(responseText);
    var temperature = Math.round(json.current.temperature_2m);
    var condition = weatherCodeToCondition(json.current.weather_code);

    var message = {};
    message[('WeatherTemperature')] = temperature;
    message[('WeatherCondition')] = condition;

    Pebble.sendAppMessage(message,
      function() { console.log('Weather sent: ' + temperature + ' ' + condition); },
      function(e) { console.log('Weather send failed: ' + JSON.stringify(e)); }
    );
  });
}

function getWeather() {

  console.log('Fetching weather...');
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
    { timeout: 15000, maximumAge: 60000 });
}

// ---------------------------------------------------------------------------
// Pebble event listeners
// ---------------------------------------------------------------------------

Pebble.addEventListener('appmessage', function(e) {
  if (e.payload.WeatherRequestUpdate) {
    getWeather();
  }
});
