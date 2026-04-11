var PANEL_OPTIONS = [
  { label: 'Companion Cube',         value: 0  },
  { label: 'Cube Drop',              value: 1  },
  { label: 'Atlas & P-Body',         value: 2  },
  { label: 'Ball Drop',              value: 3  },
  { label: 'Ball to the face',       value: 4  },
  { label: 'Bounce',                 value: 5  },
  { label: 'Cake',                   value: 6  },
  { label: 'Cube to the face',       value: 7  },
  { label: 'Dodge turret',           value: 8  },
  { label: 'Drop between portals',   value: 9  },
  { label: 'Drowning',               value: 10 },
  { label: 'GLaDOS',                 value: 11 },
  { label: 'Jump Pad',               value: 12 },
  { label: 'Laser Cube',             value: 13 },
  { label: 'Portal Bridge',          value: 14 },
  { label: 'Portal to portal fall',  value: 15 },
  { label: 'Portal flying',          value: 16 },
  { label: 'Potato',                 value: 17 },
  { label: 'Slide Run',              value: 18 },
  { label: 'Turret',                 value: 19 },
  { label: 'Wheatley',               value: 20 },
];
let DEFAULT_PANEL_1 = 15;
let DEFAULT_PANEL_2 = 1;
let DEFAULT_PANEL_3 = 8;
let DEFAULT_PANEL_4 = 16;
let DEFAULT_PANEL_5 = 2;
let DEFAULT_PANEL_6 = 10;
let DEFAULT_PANEL_7 = 14;
let DEFAULT_PANEL_8 = 4;

function buildTable(columns, rows, isHideFinal) { 
  var result = "<p>Layout of the panels on the watch</p><br/>";
  result += `<table style="border: 1px solid black; width: 100%; table-layout: fixed;">`;
  for (let r = 0; r < rows; r++) { 
    result += `<tr style="border: 1px solid black;">`;
    for (let c = 1; c <= columns; c++) { 
      if (r == rows - 1 && c == columns && isHideFinal) { 
        result += `<th style="border: 1px solid black;"></th>`;
      } else { 
        result += `<th style="border: 1px solid black;">Panel ${(r * columns) + c}</th>`;
      }
    }
    result += `</tr>`;
  }
  result += `</table>`;
  return result;
}

let header = {
  type: 'heading',
  defaultValue: 'Panels',
}

function configurationSquareSmall() { 
  return { 
    type: 'section',
    capabilities: ["DISPLAY_144x168"],
    items: [
      header,
      {
        type: "text",
        defaultValue: buildTable(3, 1, false)
      },
      {
        type: "select",
        messageKey: "PANEL_1",
        defaultValue: DEFAULT_PANEL_1,
        label: "Panel 1",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_2",
        defaultValue: DEFAULT_PANEL_2,
        label: "Panel 2",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_3",
        defaultValue: DEFAULT_PANEL_3,
        label: "Panel 3",
        options: PANEL_OPTIONS
      }
    ]
  }
}

function configurationSquareLarge() { 
  return { 
    type: 'section',
    capabilities: ["DISPLAY_200x228"],
    items: [
      header,
      {
        type: "text",
        defaultValue: buildTable(4, 2, false)
      },
      {
        type: "select",
        messageKey: "PANEL_5",
        defaultValue: DEFAULT_PANEL_5,
        label: "Panel 1",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_6",
        defaultValue: DEFAULT_PANEL_6,
        label: "Panel 2",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_7",
        defaultValue: DEFAULT_PANEL_7,
        label: "Panel 3",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_8",
        defaultValue: DEFAULT_PANEL_8,
        label: "Panel 4",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_1",
        defaultValue: DEFAULT_PANEL_1,
        label: "Panel 5",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_2",
        defaultValue: DEFAULT_PANEL_2,
        label: "Panel 6",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_3",
        defaultValue: DEFAULT_PANEL_3,
        label: "Panel 7",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_4",
        defaultValue: DEFAULT_PANEL_4,
        label: "Panel 8",
        options: PANEL_OPTIONS
      }
    ]
  }
}

function configurationRoundSmall() { 
  return { 
    type: 'section',
    capabilities: ["DISPLAY_180x180_ROUND"],
    items: [
      header,
      {
        type: "text",
        defaultValue: buildTable(3, 1, true)
      },
      {
        type: "select",
        messageKey: "PANEL_1",
        defaultValue: DEFAULT_PANEL_1,
        label: "Panel 1",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_2",
        defaultValue: DEFAULT_PANEL_2,
        label: "Panel 2",
        options: PANEL_OPTIONS
      }
    ]
  }
}

function configurationRoundLarge() { 
  return { 
    type: 'section',
    capabilities: ["DISPLAY_260x260_ROUND"],
    items: [
      header,
      {
        type: "text",
        defaultValue: buildTable(4, 2, true)
      },
      {
        type: "select",
        messageKey: "PANEL_5",
        defaultValue: DEFAULT_PANEL_5,
        label: "Panel 1",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_6",
        defaultValue: DEFAULT_PANEL_6,
        label: "Panel 2",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_7",
        defaultValue: DEFAULT_PANEL_7,
        label: "Panel 3",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_8",
        defaultValue: DEFAULT_PANEL_8,
        label: "Panel 4",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_1",
        defaultValue: DEFAULT_PANEL_1,
        label: "Panel 5",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_2",
        defaultValue: DEFAULT_PANEL_2,
        label: "Panel 6",
        options: PANEL_OPTIONS
      },
      {
        type: "select",
        messageKey: "PANEL_3",
        defaultValue: DEFAULT_PANEL_3,
        label: "Panel 7",
        options: PANEL_OPTIONS
      }
    ]
  }
}

module.exports = [
  {
    type: 'heading',
    defaultValue: 'Test Chamber',
    size: 2,
  },
  {
    type: 'text',
    defaultValue: 'Configure below',
    size: 5,
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Battery Indicator',
      },
      {
        type: 'text',
        defaultValue: 'When enabled, the progress bar between the date and the panels will represent the current battery level',
      },
      {
        type: 'toggle',
        messageKey: 'BATTERY_INDICATOR',
        label: 'Show battery percentage',
        defaultValue: true,
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Time',
      },
      {
        type: 'toggle',
        messageKey: 'AMPM_INDICATOR',
        label: 'Show AM/PM label',
        description: 'Only applies when using 12h time format',
        defaultValue: true,
      },
      {
        type: 'select',
        messageKey: 'DATE_FORMAT',
        label: 'Date format',
        defaultValue: '0',
        options: [
          { label: 'dd/mm', value: '0' },
          { label: 'mm/dd', value: '1' },
          { label: 'dd MMM', value: '2' },
        ],
      }
    ]
  },
  configurationSquareSmall(),
  configurationSquareLarge(),
  configurationRoundSmall(),
  configurationRoundLarge(),
  {
    type: 'submit',
    defaultValue: 'Save',
  },
];
