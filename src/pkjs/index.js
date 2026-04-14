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

Pebble.addEventListener('ready', function() {
  getWeather();
});

Pebble.addEventListener('appmessage', function(e) {
  if (e.payload.WeatherRequestUpdate) {
    getWeather();
  }
});
