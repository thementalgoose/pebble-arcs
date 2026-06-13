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
} MetricOption;

typedef struct {
  char   label[16];
  int    percent;
} MetricResult;

// Update the goals used for percentage calculations.
void metrics_set_goals(int step_goal, int calorie_goal, int hr_lower, int hr_upper);

// Update the temperature range used for the arc percentage.
void metrics_set_temperature_bounds(int lower, int upper);

// Update the distance goal used for the arc percentage.
void metrics_set_distance_goal(int goal);

// Restore any cached values loaded from persistent storage.
void metrics_restore_cached_values(void);

// Fetch a fresh MetricResult for the given option.
MetricResult metrics_fetch(MetricOption option);
