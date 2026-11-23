#include <pebble.h>
#include <string.h>
#include "metrics.h"
#include "constants.h"
#include "weather.h"

static int s_cached_heart_rate = -1;
static int s_phone_battery = -1;

static int  s_comp_pct[COMPLICATION_SLOTS + 1];
static char s_comp_label[COMPLICATION_SLOTS + 1][8];

void metrics_set_complication(int slot, int percent, const char *label) {
  if (slot < 1 || slot > COMPLICATION_SLOTS) return;
  if (percent > 100) percent = 100;
  if (percent < COMPLICATION_NO_PERMISSION) percent = COMPLICATION_UNSUPPORTED;
  char buf[8];
  snprintf(buf, sizeof(buf), "%s", label ? label : "");
  if (s_comp_pct[slot] == percent && strcmp(s_comp_label[slot], buf) == 0) return;
  s_comp_pct[slot] = percent;
  snprintf(s_comp_label[slot], sizeof(s_comp_label[slot]), "%s", buf);
  persist_write_int(PERSIST_KEY_COMPLICATION_PCT_BASE + slot, percent);
  persist_write_string(PERSIST_KEY_COMPLICATION_LABEL_BASE + slot, buf);
}

void metrics_set_phone_battery(int percent) {
  if (percent > 100) percent = 100;
  if (percent < 0) percent = -1;
  if (percent == s_phone_battery) return;
  s_phone_battery = percent;
  persist_write_int(PERSIST_KEY_CACHED_PHONE_BATTERY, percent);
}

static void metrics_cache_heart_rate(int bpm) {
  if (s_cached_heart_rate != -1 && s_cached_heart_rate == bpm) return;
  s_cached_heart_rate = bpm;
  persist_write_int(PERSIST_KEY_CACHED_HEART_RATE, bpm);
}

void metrics_restore_cached_values(void) {
  for (int s = 1; s <= COMPLICATION_SLOTS; s++) {
    s_comp_pct[s] = COMPLICATION_UNSUPPORTED;
    snprintf(s_comp_label[s], sizeof(s_comp_label[s]), "-");
    if (persist_exists(PERSIST_KEY_COMPLICATION_PCT_BASE + s) &&
        persist_exists(PERSIST_KEY_COMPLICATION_LABEL_BASE + s)) {
      s_comp_pct[s] = persist_read_int(PERSIST_KEY_COMPLICATION_PCT_BASE + s);
      persist_read_string(PERSIST_KEY_COMPLICATION_LABEL_BASE + s,
                          s_comp_label[s], sizeof(s_comp_label[s]));
    }
  }
  if (persist_exists(PERSIST_KEY_CACHED_PHONE_BATTERY)) {
    int stored = persist_read_int(PERSIST_KEY_CACHED_PHONE_BATTERY);
    s_phone_battery = (stored >= 0 && stored <= 100) ? stored : -1;
  }
  if (persist_exists(PERSIST_KEY_CACHED_HEART_RATE)) {
    int stored = persist_read_int(PERSIST_KEY_CACHED_HEART_RATE);
    s_cached_heart_rate = (stored > 0) ? stored : -1;
  }
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
    ? (int)health_service_peek_current_value(metric) : -1;
}

// ---------------------------------------------------------------------------
// Fetch
// ---------------------------------------------------------------------------

static int percent_in_range(int value, int min_val, int max_val) {
  int range = max_val - min_val;
  if (range <= 0) return 0;
  return CLAMP((value - min_val) * 100 / range, 0, 100);
}

MetricResult metrics_fetch(MetricOption option, int min_val, int max_val) {
  MetricResult r;
  snprintf(r.label, sizeof(r.label), "-");
  r.percent = 0;

  switch (option) {
    case METRIC_WEEK: {
      time_t now = time(NULL);
      int week = localtime(&now)->tm_yday / 7 + 1;
      snprintf(r.label, sizeof(r.label), "%d", week);
      r.percent = percent_in_range(week, min_val, max_val);
      break;
    }
    case METRIC_WEEKDAY: {
      time_t now = time(NULL);
      int wday = localtime(&now)->tm_wday;
      static const char *day_labels[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
      snprintf(r.label, sizeof(r.label), "%s", day_labels[wday]);
      // Week starts Monday: Mon=1/7 ... Sat=6/7, Sun=7/7
      int day_of_week = (wday == 0) ? 7 : wday;
      r.percent = percent_in_range(day_of_week, min_val, max_val);
      break;
    }
    case METRIC_MONTH: {
      time_t now = time(NULL);
      int month = localtime(&now)->tm_mon + 1;
      snprintf(r.label, sizeof(r.label), "%d", month);
      r.percent = percent_in_range(month, min_val, max_val);
      break;
    }
    case METRIC_DAY: {
      time_t now = time(NULL);
      int day = localtime(&now)->tm_mday;
      snprintf(r.label, sizeof(r.label), "%d", day);
      r.percent = percent_in_range(day, min_val, max_val);
      break;
    }
    case METRIC_BATTERY: {
      BatteryChargeState state = battery_state_service_peek();
      snprintf(r.label, sizeof(r.label), "%d%%", state.charge_percent);
      r.percent = percent_in_range(state.charge_percent, min_val, max_val);
      break;
    }
    case METRIC_PHONE_BATTERY: {
      if (s_phone_battery >= 0) {
        snprintf(r.label, sizeof(r.label), "%d%%", s_phone_battery);
        r.percent = percent_in_range(s_phone_battery, min_val, max_val);
      }
      break;
    }
    case METRIC_COMPLICATION_1:
    case METRIC_COMPLICATION_2:
    case METRIC_COMPLICATION_3:
    case METRIC_COMPLICATION_4:
    case METRIC_COMPLICATION_5: {
      int slot = (int)option - METRIC_COMPLICATION_1 + 1;
      // Before the first value arrives the label stays "-" with an empty arc
      if (s_comp_label[slot][0] != '\0') {
        snprintf(r.label, sizeof(r.label), "%s", s_comp_label[slot]);
      }
      r.percent = (s_comp_pct[slot] >= 0) ? percent_in_range(s_comp_pct[slot], min_val, max_val) : 0;
      break;
    }
    case METRIC_HEART_RATE: {
      int bpm = health_peek_current(HealthMetricHeartRateBPM);
      if (bpm > 0) {
        metrics_cache_heart_rate(bpm);
      } else if (s_cached_heart_rate > 0) {
        bpm = s_cached_heart_rate;
      }
      bool has_hr = bpm > 0;
      snprintf(r.label, sizeof(r.label), has_hr ? "%d" : "-", bpm);
      r.percent = has_hr ? percent_in_range(bpm, min_val, max_val) : 0;
      break;
    }
    case METRIC_STEPS: {
      int steps = health_sum_today(HealthMetricStepCount);
      if (steps > 9999) {
        snprintf(r.label, sizeof(r.label), "%dk", (steps + 500) / 1000);
      } else {
        snprintf(r.label, sizeof(r.label), "%d", steps);
      }
      r.percent = percent_in_range(steps, min_val, max_val);
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
      r.percent = percent_in_range(meters, min_val, max_val);
      break;
    }
    case METRIC_CALORIES: {
      int kcal = health_sum_today(HealthMetricActiveKCalories);
      if (kcal > 9999) {
        snprintf(r.label, sizeof(r.label), "%dk", (kcal + 500) / 1000);
      } else {
        snprintf(r.label, sizeof(r.label), "%d", kcal);
      }
      r.percent = percent_in_range(kcal, min_val, max_val);
      break;
    }
    case METRIC_TEMPERATURE: {
      if (weather_has_temperature_data()) {
        int  temp    = weather_get_temperature();
        bool celsius = weather_get_use_celsius();
        if (!celsius) temp = temp * 9 / 5 + 32;
        snprintf(r.label, sizeof(r.label), "%d%s", temp, celsius ? "°C" : "°F");
        r.percent = percent_in_range(temp, min_val, max_val);
      } else {
        snprintf(r.label, sizeof(r.label), "-");
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
          r.percent = percent_in_range(7 - cond, min_val, max_val);
        } else {
          snprintf(r.label, sizeof(r.label), "-");
          r.percent = 0;
        }
      } else {
        snprintf(r.label, sizeof(r.label), "-");
        r.percent = 0;
      }
      break;
    }
    default:
      break;
  }
  return r;
}
