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
  if (code === 0)  return 'CLOU';  //  'Clear';
  if (code === 1)  return 'CLR';  //  'MClear';
  if (code === 2)  return 'CLOU'; //  'Cloudy';
  if (code === 3)  return 'OVER';  //  'Ovrcast';
  if (code <= 48)  return 'FOG';  //  'Fog';
  if (code <= 55)  return 'DRZZ'; //  'Drizz';
  if (code <= 57)  return 'DRZZ'; //  'FrzDrz';
  if (code <= 65)  return 'RAIN'; //  'Rain';
  if (code <= 67)  return 'RAIN'; //  'FrzRain';
  if (code <= 75)  return 'SNOW'; //  'Snow';
  if (code === 77) return 'SNOW'; //  'Snow';
  if (code <= 82)  return 'RAIN'; //  'Showers';
  if (code <= 86)  return 'SNOW'; //  'SnwShr';
  return 'STRM';  // 'TStorm';
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

Pebble.addEventListener('ready', function() {
  getWeather();
});

Pebble.addEventListener('appmessage', function(e) {
  if (e.payload.WeatherRequestUpdate) {
    getWeather();
  }
});
