var Clay = require('@rebble/clay');
var clayConfig = require('./config');

var clay = new Clay(clayConfig);

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
// WMO weather code → short condition string (max 7 chars)
// ---------------------------------------------------------------------------

function weatherCodeToCondition(code) {
  if (code === 0)  return 'Clear';
  if (code === 1)  return 'MClear';
  if (code === 2)  return 'Cloudy';
  if (code === 3)  return 'Ovrcast';
  if (code <= 48)  return 'Fog';
  if (code <= 55)  return 'Drizz';
  if (code <= 57)  return 'FrzDrz';
  if (code <= 65)  return 'Rain';
  if (code <= 67)  return 'FrzRain';
  if (code <= 75)  return 'Snow';
  if (code === 77) return 'Snow';
  if (code <= 82)  return 'Showers';
  if (code <= 86)  return 'SnwShr';
  return 'T-Storm';
}

// ---------------------------------------------------------------------------
// Weather fetch
// ---------------------------------------------------------------------------

function locationError(err) {
  console.log('Location error: ' + err.message);
}

function locationSuccess(pos) {
  var settings = clay.getSettings();
  var useCelsius = (settings.WeatherUseCelsius !== false);

  var url = 'https://api.open-meteo.com/v1/forecast' +
    '?latitude=' + pos.coords.latitude +
    '&longitude=' + pos.coords.longitude +
    '&current=temperature_2m,weather_code' +
    (useCelsius ? '' : '&temperature_unit=fahrenheit');

  xhrRequest(url, 'GET', function(responseText) {
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
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
    { timeout: 15000, maximumAge: 60000 });
}

// ---------------------------------------------------------------------------
// Pebble event listeners
// ---------------------------------------------------------------------------

Pebble.addEventListener('ready', function() {
  getWeather();
});

Pebble.addEventListener('appmessage', function(e) {
  if (e.payload.WeatherRequestUpdate) {
    getWeather();
  }
});
