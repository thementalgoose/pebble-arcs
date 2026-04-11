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
        defaultValue: 'Quadrant - Top Left',
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
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Quadrant - Top Right',
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
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Quadrant - Bottom Left',
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
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Quadrant - Bottom Right',
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
    type: 'submit',
    defaultValue: 'Save',
  },
];
