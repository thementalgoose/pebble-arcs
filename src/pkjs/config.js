let options = [
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

let DEFAULT_DARK_THEME = true;
let DEFAULT_STANDALONE_BATTERY_PERCENTAGE = false;

let DEFAULT_TOP_LEFT_COLOUR = "ffffff";
let DEFAULT_TOP_LEFT_OPTION = 6;
let DEFAULT_TOP_RIGHT_COLOUR = "ffffff";
let DEFAULT_TOP_RIGHT_OPTION = 4;
let DEFAULT_BOTTOM_LEFT_COLOUR = "ffffff";
let DEFAULT_BOTTOM_LEFT_OPTION = 7;
let DEFAULT_BOTTOM_RIGHT_COLOUR = "ffffff";
let DEFAULT_BOTTOM_RIGHT_OPTION = 0;

let DEFAULT_THIN_ARCS = true;

let DEFAULT_WEATHER_UPDATE_INTERVAL = 30;
let DEFAULT_WEATHER_USE_CELSIUS = true;
let DEFAULT_TEMPERATURE_LOWER = "5";
let DEFAULT_TEMPERATURE_UPPER = "35";

let DEFAULT_STEP_GOAL = "5000";
let DEFAULT_DISTANCE_GOAL = "5000";
let DEFAULT_HEART_RATE_LOWERBOUND = "40";
let DEFAULT_HEART_RATE_UPPERBOUND = "100";
let DEFAULT_CALORIE_GOAL = "2000";

let abbreviations = [ 
  "CLR: Clear", 
  "CLD: Cloudy", 
  "OVR: Overcast", 
  "FOG: Fog", 
  "DRZZ: Drizzle",
  "RAIN: Rain", 
  "SNOW: Snow", 
  "STRM: Storm"
].map(x => `<br/>${x}`).toString()

module.exports = [
  {
    type: 'heading',
    defaultValue: 'ARCs',
    size: 2,
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Theme',
      },
      {
        type: 'toggle',
        messageKey: 'DarkTheme',
        label: 'Dark Theme',
        defaultValue: DEFAULT_DARK_THEME,
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Standalone Battery Indicator',
      },
      {
        type: 'text',
        defaultValue: 'Show a standalone battery indicator beneath the date',
      },
      {
        type: 'toggle',
        messageKey: 'BatteryIndicator',
        label: 'Show battery bar',
        defaultValue: DEFAULT_STANDALONE_BATTERY_PERCENTAGE,
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Indicators',
      },
      // {
      //   type: 'toggle',
      //   messageKey: 'ThinArcs',
      //   label: 'Thin Arc style',
      //   defaultValue: DEFAULT_THIN_ARCS,
      // },
      {
        type: "color",
        messageKey: "TopLeft_Colour",
        defaultValue: DEFAULT_TOP_LEFT_COLOUR,
        label: "Top Left - Color",
      },
      {
        type: "select",
        messageKey: "TopLeft_Option",
        defaultValue: DEFAULT_TOP_LEFT_OPTION,
        label: "Top Left - Data",
        options: options
      },
      {
        type: "color",
        messageKey: "TopRight_Colour",
        defaultValue: DEFAULT_TOP_RIGHT_COLOUR,
        label: "Top Right - Color",
      },
      {
        type: "select",
        messageKey: "TopRight_Option",
        defaultValue: DEFAULT_TOP_RIGHT_OPTION,
        label: "Top Right - Data",
        options: options
      },
      {
        type: "color",
        messageKey: "BottomLeft_Colour",
        defaultValue: DEFAULT_BOTTOM_LEFT_COLOUR,
        label: "Bottom Left - Color",
      },
      {
        type: "select",
        messageKey: "BottomLeft_Option",
        defaultValue: DEFAULT_BOTTOM_LEFT_OPTION,
        label: "Bottom Left - Data",
        options: options
      },
      {
        type: "color",
        messageKey: "BottomRight_Colour",
        defaultValue: DEFAULT_BOTTOM_RIGHT_COLOUR,
        label: "Bottom Right - Color",
      },
      {
        type: "select",
        messageKey: "BottomRight_Option",
        defaultValue: DEFAULT_BOTTOM_RIGHT_OPTION,
        label: "Bottom Right - Data",
        options: options
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Configuration',
      },
      {
        type: 'text',
        defaultValue: 'For the relevant indicators above, set goals and bounds here',
      },
      {
        type: "input",
        messageKey: "StepGoal",
        label: "Steps Goal",
        defaultValue: DEFAULT_STEP_GOAL,
      },
      {
        type: "input",
        messageKey: "HeartRateLower",
        label: "Heart Rate lower bound",
        defaultValue: DEFAULT_HEART_RATE_LOWERBOUND,
      },
      {
        type: "input",
        messageKey: "HeartRateUpper",
        label: "Heart Rate upper bound",
        defaultValue: DEFAULT_HEART_RATE_UPPERBOUND,
      },
      {
        type: "input",
        messageKey: "CalorieGoal",
        label: "Calorie Goal",
        defaultValue: DEFAULT_CALORIE_GOAL,
      },
      {
        type: "input",
        messageKey: "DistanceGoal",
        label: "Distance Goal (metres)",
        defaultValue: DEFAULT_DISTANCE_GOAL,
      },
      {
        type: "input",
        messageKey: "TemperatureLower",
        label: "Temperature lower bound",
        defaultValue: DEFAULT_TEMPERATURE_LOWER,
      },
      {
        type: "input",
        messageKey: "TemperatureUpper",
        label: "Temperature upper bound",
        defaultValue: DEFAULT_TEMPERATURE_UPPER,
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Weather',
      },
      {
        type: 'text',
        defaultValue: 'Uses GPS to fetch current weather from Open-Meteo. Select "Temperature" or "Weather" in the Indicators section to display weather data.',
      },
      {
        type: 'select',
        messageKey: 'WeatherUpdateInterval',
        label: 'Update interval',
        defaultValue: DEFAULT_WEATHER_UPDATE_INTERVAL,
        options: [
          { label: '15 minutes', value: 15 },
          { label: '30 minutes', value: 30 },
          { label: '60 minutes', value: 60 },
        ],
      },
      {
        type: 'toggle',
        messageKey: 'WeatherUseCelsius',
        label: 'Use Celsius',
        defaultValue: DEFAULT_WEATHER_USE_CELSIUS,
      },
      {
        type: 'text',
        defaultValue: `Weather condition abbrevations${abbreviations}`,
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save',
  },
];
