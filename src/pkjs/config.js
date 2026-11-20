let indicatorOptions = [
  { label: "Week of year",   value: 0 },
  { label: "Weekday",        value: 1 },
  { label: "Month",          value: 2 },
  { label: "Day of month",   value: 3 },
  { label: "Battery",        value: 4 },
  { label: "Heart Rate",     value: 5 },
  { label: "Steps",          value: 6 },
  { label: "Distance",       value: 7 },
  { label: "Calories",       value: 8 },
  { label: "Temperature",    value: 9 },
  { label: "Weather",        value: 10 },
];

// Options backed by Pebble plugin sources (shown under "Plugin Data")
let pluginOptions = [
  { label: "Phone Battery", value: 11 },
  { label: "Complication Slot 1", value: 12 },
  { label: "Complication Slot 2", value: 13 },
  { label: "Complication Slot 3", value: 14 },
  { label: "Complication Slot 4", value: 15 },
  { label: "Complication Slot 5", value: 16 },
];
let options = indicatorOptions.concat(pluginOptions);

let DEFAULT_DARK_THEME = true;

const activeWatchInfo = (typeof Pebble !== 'undefined' && Pebble.getActiveWatchInfo)
  ? Pebble.getActiveWatchInfo()
  : {};
const platform = (activeWatchInfo && activeWatchInfo.platform) ? activeWatchInfo.platform : '';
const isColorWatch = !!activeWatchInfo && activeWatchInfo.platform && activeWatchInfo.platform !== 'aplite';

let DEFAULT_INDICATOR_WIDTH = 5;
if (platform === 'emery') {
  DEFAULT_INDICATOR_WIDTH = 12;
} else if (platform === 'gabbro') {
  DEFAULT_INDICATOR_WIDTH = 10;
}

let DEFAULT_TOP_LEFT_COLOUR = isColorWatch ? "f5e39a" : "ffffff";
let DEFAULT_TOP_LEFT_OPTION = 3;
let DEFAULT_TOP_RIGHT_COLOUR = isColorWatch ? "a7d89b" : "ffffff";
let DEFAULT_TOP_RIGHT_OPTION = 1;
let DEFAULT_BOTTOM_LEFT_COLOUR = isColorWatch ? "f0a7a7" : "ffffff";
let DEFAULT_BOTTOM_LEFT_OPTION = 4;
let DEFAULT_BOTTOM_RIGHT_COLOUR = isColorWatch ? "a6d0ff" : "ffffff";
let DEFAULT_BOTTOM_RIGHT_OPTION = 0;

let DEFAULT_THIN_ARCS = true;
let DEFAULT_OUTLINED_ARCS = isColorWatch ? false : true;

let boundsData = require('./indicators.json');
let indicatorBounds = boundsData.indicators || {};
let complicationBounds = boundsData.complications || {};

let DEFAULT_WEATHER_UPDATE_INTERVAL = 60;
let DEFAULT_WEATHER_USE_CELSIUS = true;

function defaultBound(opt, key) {
  var b = indicatorBounds[String(opt)];
  return b ? b[key] : (key === 'min' ? 0 : 100);
}

let COLOR_LAYOUT = [
  [false,    "000000", false,    "FFFFFF", false   ],
  ["FFFF00", "AAAA00", "FFAA00", "FFAA55", "FF5500"],
  ["FF0000", "AA0000", "FF0055", "FF55AA", "AA0055"],
  ["FF00AA", "FF00FF", "AA00FF", "AA00AA", "FF55FF"],
  ["5500AA", "5500FF", "0000FF", "0055AA", "00AAFF"],
  ["00FFFF", "00AAAA", "55AAAA", "00FF00", "00AA00"],
  ["00AA55", "55FF00", "55AA00", "55AA55", false   ]
]

let abbreviations = [ 
  "CLR: Clear", 
  "CLD: Cloudy", 
  "OVR: Overcast", 
  "FOG: Fog", 
  "DRZZ: Drizzle",
  "RAIN: Rain", 
  "SNOW: Snow", 
  "STRM: Storm"
]


module.exports = {
  options: options,
  indicatorOptions: indicatorOptions,
  pluginOptions: pluginOptions,
  indicatorBounds: indicatorBounds,
  complicationBounds: complicationBounds,
  colorLayout: COLOR_LAYOUT,
  abbreviations: abbreviations,
  isColorWatch: isColorWatch,
  defaults: {
    DarkTheme: DEFAULT_DARK_THEME,
    QuietTimeIndicator: true,
    DisconnectIndicator: true,
    OutlinedArcs: DEFAULT_OUTLINED_ARCS,
    ShowIndicatorText: true,
    AnimateIndicators: false,
    IndicatorWidth: DEFAULT_INDICATOR_WIDTH,
    TopLeft_Colour: DEFAULT_TOP_LEFT_COLOUR,
    TopLeft_Option: DEFAULT_TOP_LEFT_OPTION,
    TopLeft_Min: defaultBound(DEFAULT_TOP_LEFT_OPTION, 'min'),
    TopLeft_Max: defaultBound(DEFAULT_TOP_LEFT_OPTION, 'max'),
    TopRight_Colour: DEFAULT_TOP_RIGHT_COLOUR,
    TopRight_Option: DEFAULT_TOP_RIGHT_OPTION,
    TopRight_Min: defaultBound(DEFAULT_TOP_RIGHT_OPTION, 'min'),
    TopRight_Max: defaultBound(DEFAULT_TOP_RIGHT_OPTION, 'max'),
    BottomLeft_Colour: DEFAULT_BOTTOM_LEFT_COLOUR,
    BottomLeft_Option: DEFAULT_BOTTOM_LEFT_OPTION,
    BottomLeft_Min: defaultBound(DEFAULT_BOTTOM_LEFT_OPTION, 'min'),
    BottomLeft_Max: defaultBound(DEFAULT_BOTTOM_LEFT_OPTION, 'max'),
    BottomRight_Colour: DEFAULT_BOTTOM_RIGHT_COLOUR,
    BottomRight_Option: DEFAULT_BOTTOM_RIGHT_OPTION,
    BottomRight_Min: defaultBound(DEFAULT_BOTTOM_RIGHT_OPTION, 'min'),
    BottomRight_Max: defaultBound(DEFAULT_BOTTOM_RIGHT_OPTION, 'max'),
    WeatherUpdateInterval: DEFAULT_WEATHER_UPDATE_INTERVAL,
    WeatherUseCelsius: DEFAULT_WEATHER_USE_CELSIUS,
  },
};
