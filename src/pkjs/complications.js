// Converts a plugin property (a map of shapes) into what the watch displays.
// Shape preference: numericValue > boolean > unsupported.
// percent: 0-100, or -1 (unsupported) / -2 (permission not allowed)
var UNSUPPORTED = { percent: -1, label: '--' };
var NO_PERMISSION = { percent: -2, label: '-' };
var LABEL_MAX = 7;

function clamp(n, lo, hi) { return Math.max(lo, Math.min(hi, n)); }

function formatNumber(v, unit) {
  var s = String(Math.round(v * 10) / 10);
  return unit ? s + unit : s;
}

function trimLabel(s) {
  s = String(s);
  return s.length > LABEL_MAX ? s.slice(0, LABEL_MAX) : s;
}

function toDisplay(prop) {
  if (!prop || typeof prop !== 'object') return UNSUPPORTED;
  var shortText = prop.shortText && typeof prop.shortText.text === 'string'
    ? prop.shortText.text : null;

  var n = prop.numericValue;
  if (n && typeof n.value === 'number' && isFinite(n.value)) {
    var percent = 0;
    if (typeof n.min === 'number' && typeof n.max === 'number' && n.max > n.min) {
      percent = (n.value - n.min) * 100 / (n.max - n.min);
    } else if (n.unit === '%') {
      percent = n.value;
    }
    return {
      percent: clamp(Math.round(percent), 0, 100),
      label: trimLabel(shortText !== null ? shortText : formatNumber(n.value, n.unit))
    };
  }

  var b = prop.boolean;
  if (b && typeof b.value === 'boolean') {
    return {
      percent: b.value ? 100 : 0,
      label: trimLabel(shortText !== null ? shortText : (b.value ? 'On' : 'Off'))
    };
  }

  return UNSUPPORTED;
}

module.exports = {
  toDisplay: toDisplay,
  UNSUPPORTED: UNSUPPORTED,
  NO_PERMISSION: NO_PERMISSION
};
