#pragma once
#include <pebble.h>

// Option values — must match the `options` array order in config.js
typedef enum {
  METRIC_WEEK        = 0,
  METRIC_WEEKDAY     = 1,
  METRIC_MONTH       = 2,
  METRIC_DAY         = 3,
  METRIC_BATTERY     = 4,
  METRIC_HEART_RATE  = 5,
  METRIC_STEPS       = 6,
  METRIC_DISTANCE    = 7,
  METRIC_CALORIES    = 8,
  METRIC_TEMPERATURE        = 9,
  METRIC_WEATHER_CONDITION  = 10,
  METRIC_PHONE_BATTERY      = 11,  // plugin: phone/phone_state battery_level
  METRIC_COMPLICATION_1     = 12,  // plugin: user's complication slots 1..5
  METRIC_COMPLICATION_2     = 13,
  METRIC_COMPLICATION_3     = 14,
  METRIC_COMPLICATION_4     = 15,
  METRIC_COMPLICATION_5     = 16,
} MetricOption;

typedef struct {
  char   label[16];
  int    percent;
} MetricResult;

// Update the phone battery level (0-100, or negative when unavailable).
void metrics_set_phone_battery(int percent);

#define COMPLICATION_SLOTS 5
// Complication percent: 0-100, or COMPLICATION_UNSUPPORTED / COMPLICATION_NO_PERMISSION.
#define COMPLICATION_UNSUPPORTED  (-1)
#define COMPLICATION_NO_PERMISSION (-2)

// Store a complication slot's display value (slot is 1-based) and persist it.
void metrics_set_complication(int slot, int percent, const char *label);

// Restore any cached values loaded from persistent storage.
void metrics_restore_cached_values(void);

// Fetch a fresh MetricResult for the given option using the configured [min_val, max_val] range.
MetricResult metrics_fetch(MetricOption option, int min_val, int max_val);
