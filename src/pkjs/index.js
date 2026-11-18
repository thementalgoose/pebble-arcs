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
  });
  return s;
}

function toWire(s) {
  var m = {};
  ['DarkTheme', 'BatteryIndicator', 'QuietTimeIndicator', 'DisconnectIndicator',
   'OutlinedArcs', 'ShowIndicatorText', 'WeatherUseCelsius'].forEach(function(k) {
    m[k] = s[k] ? 1 : 0;
  });
  ['IndicatorWidth', 'WeatherUpdateInterval', 'TopLeft_Option', 'TopRight_Option',
   'BottomLeft_Option', 'BottomRight_Option'].forEach(function(k) {
    m[k] = parseInt(s[k], 10);
  });
  ['TopLeft_Colour', 'TopRight_Colour', 'BottomLeft_Colour', 'BottomRight_Colour'].forEach(function(k) {
    m[k] = parseInt(s[k], 16);
  });
  // The watch parses these with atoi(cstring), so send them as strings
  ['StepGoal', 'HeartRateLower', 'HeartRateUpper', 'CalorieGoal', 'DistanceGoal',
   'TemperatureLower', 'TemperatureUpper'].forEach(function(k) {
    var n = parseInt(s[k], 10);
    m[k] = String(isNaN(n) ? cfg.defaults[k] : n);
  });
  return m;
}

// ---------------------------------------------------------------------------
// Plugin data sources (Pebble Plugins preview, mobile app 2.0.0+)
// ---------------------------------------------------------------------------

var QUADRANT_KEYS = ['TopLeft', 'TopRight', 'BottomLeft', 'BottomRight'];
var OPTION_PHONE_BATTERY = 11;
var phoneBatterySub = null;

function sendPhoneBattery(value) {
  Pebble.sendAppMessage({ PhoneBattery: value },
    function() { console.log('Phone battery sent: ' + value); },
    function(e) { console.log('Phone battery send failed: ' + JSON.stringify(e)); });
}

function syncPluginSubscriptions(settings) {
  var wanted = QUADRANT_KEYS.some(function(q) {
    return parseInt(settings[q + '_Option'], 10) === OPTION_PHONE_BATTERY;
  });

  if (wanted && !phoneBatterySub) {
    if (typeof Pebble.subscribeToSource !== 'function') {
      console.log('Plugins not supported by this Pebble app');
      sendPhoneBattery(-1);
      return;
    }
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
  } else if (!wanted && phoneBatterySub) {
    phoneBatterySub.unsubscribe();
    phoneBatterySub = null;
  }
}

Pebble.addEventListener('ready', function() {
  syncPluginSubscriptions(loadSettings());
});

Pebble.addEventListener('showConfiguration', function() {
  var html = configPage.build(loadSettings());
  Pebble.openURL('data:text/html;charset=utf-8,' + encodeURIComponent(html));
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
