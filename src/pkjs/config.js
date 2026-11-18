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

let DEFAULT_WEATHER_UPDATE_INTERVAL = 60;
let DEFAULT_WEATHER_USE_CELSIUS = true;
let DEFAULT_TEMPERATURE_LOWER = "5";
let DEFAULT_TEMPERATURE_UPPER = "35";

let DEFAULT_STEP_GOAL = "5000";
let DEFAULT_DISTANCE_GOAL = "5000";
let DEFAULT_HEART_RATE_LOWERBOUND = "40";
let DEFAULT_HEART_RATE_UPPERBOUND = "100";
let DEFAULT_CALORIE_GOAL = "2000";

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
    TopRight_Colour: DEFAULT_TOP_RIGHT_COLOUR,
    TopRight_Option: DEFAULT_TOP_RIGHT_OPTION,
    BottomLeft_Colour: DEFAULT_BOTTOM_LEFT_COLOUR,
    BottomLeft_Option: DEFAULT_BOTTOM_LEFT_OPTION,
    BottomRight_Colour: DEFAULT_BOTTOM_RIGHT_COLOUR,
    BottomRight_Option: DEFAULT_BOTTOM_RIGHT_OPTION,
    StepGoal: DEFAULT_STEP_GOAL,
    HeartRateLower: DEFAULT_HEART_RATE_LOWERBOUND,
    HeartRateUpper: DEFAULT_HEART_RATE_UPPERBOUND,
    CalorieGoal: DEFAULT_CALORIE_GOAL,
    DistanceGoal: DEFAULT_DISTANCE_GOAL,
    TemperatureLower: DEFAULT_TEMPERATURE_LOWER,
    TemperatureUpper: DEFAULT_TEMPERATURE_UPPER,
    WeatherUpdateInterval: DEFAULT_WEATHER_UPDATE_INTERVAL,
    WeatherUseCelsius: DEFAULT_WEATHER_USE_CELSIUS,
  },
};
