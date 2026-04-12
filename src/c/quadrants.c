#include <pebble.h>
#include "quadrants.h"
#include "metrics.h"
#include "indicators.h"

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

// Default options per quadrant — must match DEFAULT_* values in config.js
static const int k_default_options[QUADRANT_COUNT] = {
  [QUADRANT_NW] = METRIC_DAY,
  [QUADRANT_NE] = METRIC_HEART_RATE,
  [QUADRANT_SW] = METRIC_CALORIES,
  [QUADRANT_SE] = METRIC_WEEK,
};

static int    s_options[QUADRANT_COUNT];
static GColor s_colors[QUADRANT_COUNT];

// ---------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------

static void render_quadrant(Quadrant q) {
  MetricResult r     = metrics_fetch((MetricOption)s_options[q]);
  GColor       color = gcolor_equal(s_colors[q], GColorClear) ? r.color : s_colors[q];
  printf("Rendering quadrant %d: option=%d, label=%s, percent=%d, color=%02x\n",
         q, s_options[q], r.label, r.percent, color.argb);
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
    s_options[q] = (option >= METRIC_WEEK && option <= METRIC_TEMPERATURE)
      ? option : k_default_options[q];

    uint32_t col_key = colour_key_for((Quadrant)q);
    s_colors[q] = persist_exists(col_key)
      ? ((GColor){ .argb = (uint8_t)persist_read_int(col_key) })
      : GColorClear;
  }
}

void quadrants_render_all(void) {
  for (int q = 0; q < QUADRANT_COUNT; q++) {
    render_quadrant((Quadrant)q);
  }
}

void quadrants_set_option(Quadrant q, int option) {
  s_options[q] = option;
  persist_write_int(option_key_for(q), option);
}

void quadrants_set_color(Quadrant q, GColor color) {
  s_colors[q] = color;
  persist_write_int(colour_key_for(q), color.argb);
}
