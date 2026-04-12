let options = [
  { label: "Day Of Week", value: 0 },
  { label: "Month",       value: 1 },
  { label: "Battery",     value: 2 },
  { label: "Heart Rate",  value: 3 },
  { label: "Steps",       value: 4 },
  { label: "Distance",    value: 5 },
  { label: "Calories",    value: 6 },
  { label: "Temperature", value: 7 },
  { label: "Temperature", value: 7 },
];

let DEFAULT_DARK_THEME = true;
let DEFAULT_STANDALONE_BATTERY_PERCENTAGE = false;

let DEFAULT_TOP_LEFT_COLOUR = "ff0000";
let DEFAULT_TOP_LEFT_OPTION = 2;
let DEFAULT_TOP_RIGHT_COLOUR = "ff0000";
let DEFAULT_TOP_RIGHT_OPTION = 4;
let DEFAULT_BOTTOM_LEFT_COLOUR = "ff0000";
let DEFAULT_BOTTOM_LEFT_OPTION = 7;
let DEFAULT_BOTTOM_RIGHT_COLOUR = "ff0000";
let DEFAULT_BOTTOM_RIGHT_OPTION = 0;

let DEFAULT_STEP_GOAL = "5000";
let DEFAULT_HEART_RATE_LOWERBOUND = "40";
let DEFAULT_HEART_RATE_UPPERBOUND = "100";
let DEFAULT_CALORIE_GOAL = "2000";

module.exports = [
  {
    type: 'heading',
    defaultValue: 'Quad Watch',
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
    type: 'heading',
    defaultValue: 'Indicators',
    size: 4,
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Quadrants',
      },
      {
        type: 'text',
        defaultValue: 'Top Left',
      },
      {
        type: "color",
        messageKey: "TopLeft_Colour",
        defaultValue: DEFAULT_TOP_LEFT_COLOUR,
        label: "Background Color",
      },
      {
        type: "select",
        messageKey: "TopLeft_Option",
        defaultValue: DEFAULT_TOP_LEFT_OPTION,
        label: "Data",
        options: options
      },
      {
        type: 'text',
        defaultValue: 'Top Right',
      },
      {
        type: "color",
        messageKey: "TopRight_Colour",
        defaultValue: DEFAULT_TOP_RIGHT_COLOUR,
        label: "Background Color",
      },
      {
        type: "select",
        messageKey: "TopRight_Option",
        defaultValue: DEFAULT_TOP_RIGHT_OPTION,
        label: "Data",
        options: options
      },
      {
        type: 'text',
        defaultValue: 'Bottom Left',
      },
      {
        type: "color",
        messageKey: "BottomLeft_Colour",
        defaultValue: DEFAULT_BOTTOM_LEFT_COLOUR,
        label: "Background Color",
      },
      {
        type: "select",
        messageKey: "BottomLeft_Option",
        defaultValue: DEFAULT_BOTTOM_LEFT_OPTION,
        label: "Data",
        options: options
      },
      {
        type: 'text',
        defaultValue: 'Bottom Right',
      },
      {
        type: "color",
        messageKey: "BottomRight_Colour",
        defaultValue: DEFAULT_BOTTOM_RIGHT_COLOUR,
        label: "Background Color",
      },
      {
        type: "select",
        messageKey: "BottomRight_Option",
        defaultValue: DEFAULT_BOTTOM_RIGHT_OPTION,
        label: "Data",
        options: options
      }
    ]
  },
  {
    type: 'heading',
    defaultValue: 'Configuration',
    size: 4,
  },
  {
    type: 'section',
    items: [
      {
        type: 'text',
        defaultValue: 'Steps',
      },
      {
        type: "input",
        messageKey: "StepGoal",
        label: "Steps Goal",
        defaultValue: DEFAULT_STEP_GOAL,
      },
      {
        type: 'text',
        defaultValue: 'Heart Rate',
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
        type: 'text',
        defaultValue: 'Calories',
      },
      {
        type: "input",
        messageKey: "CalorieGoal",
        label: "Calorie Goal",
        defaultValue: DEFAULT_CALORIE_GOAL,
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save',
  },
];
