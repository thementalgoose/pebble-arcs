#include <pebble.h>
#include "quadrants.h"
#include "metrics.h"
#include "indicators.h"
#include "constants.h"

// ---------------------------------------------------------------------------
// Persist key lookup (MESSAGE_KEY_* are extern consts, not compile-time constants)
// ---------------------------------------------------------------------------
static uint32_t option_key_for(Quadrant q) {
  switch (q) {
    case QUADRANT_NW: return MESSAGE_KEY_TopLeft_Option;
    case QUADRANT_NE: return MESSAGE_KEY_TopRight_Option;
    case QUADRANT_SW: return MESSAGE_KEY_BottomLeft_Option;
    case QUADRANT_SE: return MESSAGE_KEY_BottomRight_Option;
    default:          return 0;
  }
}

static uint32_t colour_key_for(Quadrant q) {
  switch (q) {
    case QUADRANT_NW: return MESSAGE_KEY_TopLeft_Colour;
    case QUADRANT_NE: return MESSAGE_KEY_TopRight_Colour;
    case QUADRANT_SW: return MESSAGE_KEY_BottomLeft_Colour;
    case QUADRANT_SE: return MESSAGE_KEY_BottomRight_Colour;
    default:          return 0;
  }
}

static uint32_t min_key_for(Quadrant q) {
  switch (q) {
    case QUADRANT_NW: return MESSAGE_KEY_TopLeft_Min;
    case QUADRANT_NE: return MESSAGE_KEY_TopRight_Min;
    case QUADRANT_SW: return MESSAGE_KEY_BottomLeft_Min;
    case QUADRANT_SE: return MESSAGE_KEY_BottomRight_Min;
    default:          return 0;
  }
}

static uint32_t max_key_for(Quadrant q) {
  switch (q) {
    case QUADRANT_NW: return MESSAGE_KEY_TopLeft_Max;
    case QUADRANT_NE: return MESSAGE_KEY_TopRight_Max;
    case QUADRANT_SW: return MESSAGE_KEY_BottomLeft_Max;
    case QUADRANT_SE: return MESSAGE_KEY_BottomRight_Max;
    default:          return 0;
  }
}

// Default bounds per option — must match src/pkjs/indicators.json
static void default_bounds_for(int option, int *min_out, int *max_out) {
  switch (option) {
    case METRIC_WEEK:              *min_out = 0;  *max_out = 52;   break;
    case METRIC_WEEKDAY:           *min_out = 0;  *max_out = 7;    break;
    case METRIC_MONTH:             *min_out = 0;  *max_out = 12;   break;
    case METRIC_DAY:               *min_out = 0;  *max_out = 31;   break;
    case METRIC_HEART_RATE:        *min_out = 40; *max_out = 100;  break;
    case METRIC_STEPS:             *min_out = 0;  *max_out = 5000; break;
    case METRIC_DISTANCE:          *min_out = 0;  *max_out = 5000; break;
    case METRIC_CALORIES:          *min_out = 0;  *max_out = 2000; break;
    case METRIC_TEMPERATURE:       *min_out = 5;  *max_out = 35;   break;
    case METRIC_WEATHER_CONDITION: *min_out = 0;  *max_out = 7;    break;
    default:                       *min_out = 0;  *max_out = 100;  break;
  }
}

// Default options per quadrant — must match DEFAULT_* values in config.js
static const int k_default_options[QUADRANT_COUNT] = {
  [QUADRANT_NW] = METRIC_DAY,
  [QUADRANT_NE] = METRIC_WEEKDAY,
  [QUADRANT_SW] = METRIC_BATTERY,
  [QUADRANT_SE] = METRIC_WEEK,
};

static int    s_options[QUADRANT_COUNT];
static GColor s_colors[QUADRANT_COUNT];
static int    s_min[QUADRANT_COUNT];
static int    s_max[QUADRANT_COUNT];

// ---------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------

static void render_quadrant(Quadrant q) {
  MetricResult r     = metrics_fetch((MetricOption)s_options[q], s_min[q], s_max[q]);
  GColor       color = s_colors[q];
  indicators_set(q, r.label, r.percent, color);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void quadrants_load(void) {
  for (int q = 0; q < QUADRANT_COUNT; q++) {
    uint32_t opt_key = option_key_for((Quadrant)q);
    int option = persist_exists(opt_key) ? persist_read_int(opt_key) : k_default_options[q];
    // Reject stale values outside the valid option range (e.g. from a previous key layout)
    s_options[q] = (option >= METRIC_WEEK && option <= METRIC_COMPLICATION_5)
      ? option : k_default_options[q];

    int def_min, def_max;
    default_bounds_for(s_options[q], &def_min, &def_max);
    uint32_t min_key = min_key_for((Quadrant)q);
    uint32_t max_key = max_key_for((Quadrant)q);
    s_min[q] = persist_exists(min_key) ? persist_read_int(min_key) : def_min;
    s_max[q] = persist_exists(max_key) ? persist_read_int(max_key) : def_max;

    uint32_t col_key = colour_key_for((Quadrant)q);
    if (persist_exists(col_key)) {
      s_colors[q] = (GColor){ .argb = (uint8_t)persist_read_int(col_key) };
    } else {
      const GColor k_default_colors[QUADRANT_COUNT] = {
        [QUADRANT_NW] = DEFAULT_ARC_COLOR_NW,
        [QUADRANT_NE] = DEFAULT_ARC_COLOR_NE,
        [QUADRANT_SW] = DEFAULT_ARC_COLOR_SW,
        [QUADRANT_SE] = DEFAULT_ARC_COLOR_SE,
      };
      s_colors[q] = k_default_colors[q];
    }
  }
}

void quadrants_render_all(void) {
  for (int q = 0; q < QUADRANT_COUNT; q++) {
    render_quadrant((Quadrant)q);
  }
}

void quadrants_set_option(Quadrant q, int option) {
  if (option < METRIC_WEEK || option > METRIC_COMPLICATION_5) {
    option = k_default_options[q];
  }
  s_options[q] = option;
  persist_write_int(option_key_for(q), option);
}

bool quadrants_has_weather_metric(void) {
  for (int q = 0; q < QUADRANT_COUNT; q++) {
    if (s_options[q] == METRIC_TEMPERATURE || s_options[q] == METRIC_WEATHER_CONDITION) {
      return true;
    }
  }
  return false;
}

void quadrants_set_color(Quadrant q, GColor color) {
  s_colors[q] = color;
  persist_write_int(colour_key_for(q), color.argb);
}

void quadrants_set_min(Quadrant q, int min_val) {
  s_min[q] = min_val;
  persist_write_int(min_key_for(q), min_val);
}

void quadrants_set_max(Quadrant q, int max_val) {
  s_max[q] = max_val;
  persist_write_int(max_key_for(q), max_val);
}
