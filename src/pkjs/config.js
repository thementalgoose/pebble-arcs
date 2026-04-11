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
        messageKey: 'DARK_THEME',
        label: 'Dark Theme',
        defaultValue: true,
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
        messageKey: 'BATTERY_INDICATOR',
        label: 'Show battery percentage',
        defaultValue: true,
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save',
  },
];
