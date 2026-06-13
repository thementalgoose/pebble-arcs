#include <pebble.h>
#include "metrics.h"
#include "constants.h"
#include "weather.h"

static const uint32_t PERSIST_KEY_CACHED_HEART_RATE = 0xFEF0;

static int s_step_goal    = 5000;
static int s_calorie_goal = 2000;
static int s_hr_lower     = 40;
static int s_hr_upper     = 100;
static int s_temp_lower    = 5;
static int s_temp_upper    = 35;
static int s_distance_goal = 5000;

static int  s_cached_heart_rate = 0;
static bool s_have_cached_heart_rate = false;

void metrics_set_distance_goal(int goal) {
  s_distance_goal = goal;
}

static void metrics_cache_heart_rate(int bpm) {
  if (bpm <= 0) return;
  if (s_have_cached_heart_rate && s_cached_heart_rate == bpm) return;
  s_cached_heart_rate = bpm;
  s_have_cached_heart_rate = true;
  persist_write_int(PERSIST_KEY_CACHED_HEART_RATE, bpm);
}

void metrics_restore_cached_values(void) {
  if (persist_exists(PERSIST_KEY_CACHED_HEART_RATE)) {
    int stored = persist_read_int(PERSIST_KEY_CACHED_HEART_RATE);
    if (stored > 0) {
      s_cached_heart_rate = stored;
      s_have_cached_heart_rate = true;
    }
  }
}

void metrics_set_temperature_bounds(int lower, int upper) {
  s_temp_lower = lower;
  s_temp_upper = upper;
}

void metrics_set_goals(int step_goal, int calorie_goal, int hr_lower, int hr_upper) {
  s_step_goal    = step_goal;
  s_calorie_goal = calorie_goal;
  s_hr_lower     = hr_lower;
  s_hr_upper     = hr_upper;
}

// ---------------------------------------------------------------------------
// Health helpers
// ---------------------------------------------------------------------------

static int health_sum_today(HealthMetric metric) {
  HealthServiceAccessibilityMask mask =
    health_service_metric_accessible(metric, time_start_of_today(), time(NULL));
  return (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(metric) : 0;
}

static int health_peek_current(HealthMetric metric) {
  HealthServiceAccessibilityMask mask =
    health_service_metric_accessible(metric, time(NULL), time(NULL));
  return (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_peek_current_value(metric) : 0;
}

// ---------------------------------------------------------------------------
// Fetch
// ---------------------------------------------------------------------------

MetricResult metrics_fetch(MetricOption option) {
  MetricResult r;
  snprintf(r.label, sizeof(r.label), "--");
  r.percent = 0;

  switch (option) {
    case METRIC_WEEK: {
      time_t now = time(NULL);
      int week = localtime(&now)->tm_yday / 7 + 1;
      snprintf(r.label, sizeof(r.label), "%d", week);
      r.percent = CLAMP(week * 100 / 52, 0, 100);
      break;
    }
    case METRIC_WEEKDAY: {
      time_t now = time(NULL);
      int wday = localtime(&now)->tm_wday;
      static const char *day_labels[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
      snprintf(r.label, sizeof(r.label), "%s", day_labels[wday]);
      // Week starts Monday: Mon=1/7 ... Sat=6/7, Sun=7/7
      int day_of_week = (wday == 0) ? 7 : wday;
      r.percent = day_of_week * 100 / 7;
      break;
    }
    case METRIC_MONTH: {
      time_t now = time(NULL);
      int month = localtime(&now)->tm_mon + 1;
      snprintf(r.label, sizeof(r.label), "%d", month);
      r.percent = month * 100 / 12;
      break;
    }
    case METRIC_DAY: {
      time_t now = time(NULL);
      int day = localtime(&now)->tm_mday;
      snprintf(r.label, sizeof(r.label), "%d", day);
      r.percent = CLAMP(day * 100 / 31, 0, 100);
      break;
    }
    case METRIC_BATTERY: {
      BatteryChargeState state = battery_state_service_peek();
      snprintf(r.label, sizeof(r.label), "%d%%", state.charge_percent);
      r.percent = state.charge_percent;
      break;
    }
    case METRIC_HEART_RATE: {
      int bpm = health_peek_current(HealthMetricHeartRateBPM);
      if (bpm > 0) {
        metrics_cache_heart_rate(bpm);
      } else if (s_have_cached_heart_rate) {
        bpm = s_cached_heart_rate;
      }
      snprintf(r.label, sizeof(r.label), bpm > 0 ? "%d" : "--", bpm);
      int range = s_hr_upper - s_hr_lower;
      r.percent = (range > 0) ? CLAMP((bpm - s_hr_lower) * 100 / range, 0, 100) : 0;
      break;
    }
    case METRIC_STEPS: {
      int steps = health_sum_today(HealthMetricStepCount);
      if (steps > 9999) {
        snprintf(r.label, sizeof(r.label), "%dk", (steps + 500) / 1000);
      } else {
        snprintf(r.label, sizeof(r.label), "%d", steps);
      }
      r.percent = (s_step_goal > 0) ? CLAMP(steps * 100 / s_step_goal, 0, 100) : 0;
      break;
    }
    case METRIC_DISTANCE: {
      int meters = health_sum_today(HealthMetricWalkedDistanceMeters);
      if (meters > 9999) {
        snprintf(r.label, sizeof(r.label), "%dk", (meters + 500) / 1000);
      } else if (meters >= 1000) {
        snprintf(r.label, sizeof(r.label), "%d.%dk", meters / 1000, (meters % 1000) / 100);
      } else {
        snprintf(r.label, sizeof(r.label), "%dm", meters);
      }
      r.percent = (s_distance_goal > 0) ? CLAMP(meters * 100 / s_distance_goal, 0, 100) : 0;
      break;
    }
    case METRIC_CALORIES: {
      int kcal = health_sum_today(HealthMetricActiveKCalories);
      if (kcal > 9999) {
        snprintf(r.label, sizeof(r.label), "%dk", (kcal + 500) / 1000);
      } else {
        snprintf(r.label, sizeof(r.label), "%d", kcal);
      }
      r.percent = (s_calorie_goal > 0) ? CLAMP(kcal * 100 / s_calorie_goal, 0, 100) : 0;
      break;
    }
    case METRIC_TEMPERATURE: {
      if (weather_has_temperature_data()) {
        int  temp    = weather_get_temperature();
        bool celsius = weather_get_use_celsius();
        if (!celsius) temp = temp * 9 / 5 + 32;
        snprintf(r.label, sizeof(r.label), "%d%s", temp, celsius ? "°C" : "°F");
        int range = s_temp_upper - s_temp_lower;
        r.percent = (range > 0) ? CLAMP((temp - s_temp_lower) * 100 / range, 0, 100) : 0;
      } else {
        snprintf(r.label, sizeof(r.label), "--");
        r.percent = 0;
      }
      break;
    }
    case METRIC_WEATHER_CONDITION: {
      if (weather_has_condition_data()) {
        static const char *condition_labels[] = {
          "CLR", "CLD", "OVR", "FOG", "DRZZ", "RAIN", "SNOW", "STRM"
        };
        int cond = weather_get_condition();
        if (cond >= 0 && cond <= 7) {
          snprintf(r.label, sizeof(r.label), "%s", condition_labels[cond]);
          r.percent = (7 - cond) * 100 / 7;
        } else {
          snprintf(r.label, sizeof(r.label), "--");
          r.percent = 0;
        }
      } else {
        snprintf(r.label, sizeof(r.label), "--");
        r.percent = 0;
      }
      break;
    }
    default:
      break;
  }
  return r;
}
