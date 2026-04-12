#include <pebble.h>
#include "metrics.h"
#include "constants.h"

static int s_step_goal    = 5000;
static int s_calorie_goal = 2000;
static int s_hr_lower     = 40;
static int s_hr_upper     = 100;

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
  r.color   = g_color_indicator;

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
      snprintf(r.label, sizeof(r.label), "%c", "SMTWTFS"[wday]);
      r.percent = wday * 100 / 6;
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
      r.color   = GColorOrange;
      break;
    }
    case METRIC_HEART_RATE: {
      int bpm = health_peek_current(HealthMetricHeartRateBPM);
      snprintf(r.label, sizeof(r.label), bpm > 0 ? "%d" : "--", bpm);
      int range = s_hr_upper - s_hr_lower;
      r.percent = (range > 0) ? CLAMP((bpm - s_hr_lower) * 100 / range, 0, 100) : 0;
      r.color   = GColorRed;
      break;
    }
    case METRIC_STEPS: {
      int steps = health_sum_today(HealthMetricStepCount);
      snprintf(r.label, sizeof(r.label), "%d", steps);
      r.percent = (s_step_goal > 0) ? CLAMP(steps * 100 / s_step_goal, 0, 100) : 0;
      r.color   = GColorGreen;
      break;
    }
    case METRIC_DISTANCE: {
      int meters = health_sum_today(HealthMetricWalkedDistanceMeters);
      if (meters >= 1000) {
        snprintf(r.label, sizeof(r.label), "%d.%dk", meters / 1000, (meters % 1000) / 100);
      } else {
        snprintf(r.label, sizeof(r.label), "%dm", meters);
      }
      r.percent = CLAMP(meters * 100 / 8000, 0, 100);
      r.color   = GColorCyan;
      break;
    }
    case METRIC_CALORIES: {
      int kcal = health_sum_today(HealthMetricActiveKCalories);
      snprintf(r.label, sizeof(r.label), "%d", kcal);
      r.percent = (s_calorie_goal > 0) ? CLAMP(kcal * 100 / s_calorie_goal, 0, 100) : 0;
      r.color   = GColorYellow;
      break;
    }
    case METRIC_TEMPERATURE:
    default:
      break;
  }
  printf("Fetching metric %d: label=%s, percent=%d, color=%02x\n", option, r.label, r.percent, r.color.argb);
  return r;
}
