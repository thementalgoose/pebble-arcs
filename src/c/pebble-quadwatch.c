#include <pebble.h>
#include "battery.h"
#include "constants.h"
#include "date.h"
#include "indicators.h"
#include "time.h"

// ---------------------------------------------------------------------------
// Runtime theme color globals (declared extern in constants.h)
// ---------------------------------------------------------------------------
GColor g_color_background;
GColor g_color_bar;
GColor g_color_hour;
GColor g_color_minute;
GColor g_color_indicator;

static Window *s_window;

static void theme_apply(bool dark) {
  persist_write_bool(MESSAGE_KEY_DarkTheme, dark);
  if (dark) {
    g_color_background = GColorBlack;
    g_color_bar        = GColorWhite;
    g_color_hour       = GColorWhite;
    g_color_minute     = GColorWhite;
    g_color_indicator  = GColorWhite;
  } else {
    g_color_background = GColorWhite;
    g_color_bar        = GColorBlack;
    g_color_hour       = GColorBlack;
    g_color_minute     = GColorBlack;
    g_color_indicator  = GColorDarkGray;
  }
  if (s_window && window_is_loaded(s_window)) {
    window_set_background_color(s_window, g_color_background);
    time_layer_apply_theme();
    date_layer_apply_theme();
    battery_layer_apply_theme();
    indicators_layer_apply_theme();
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *dark_theme_t = dict_find(iter, MESSAGE_KEY_DarkTheme);
  if (dark_theme_t) {
    theme_apply(dark_theme_t->value->int32 != 0);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  time_layer_update(tick_time);
  date_layer_update(tick_time);
}

static void battery_handler(BatteryChargeState state) {
  battery_layer_set(state.charge_percent);
  char buf[8];
  snprintf(buf, sizeof(buf), "%d%%", state.charge_percent);
  indicators_set_ne(buf, state.charge_percent, GColorOrange);
}

static void update_step_count(void) {
  HealthMetric metric = HealthMetricStepCount;
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, time_start_of_today(), time(NULL));
  int steps = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(metric)
    : 0;
  char buf[8];
  snprintf(buf, sizeof(buf), "%d", steps);
  int percent = CLAMP(steps * 100 / 10000, 0, 100);
  indicators_set_nw(buf, percent, GColorGreen);
}

static void update_heart_rate(void) {
  HealthMetric metric = HealthMetricHeartRateBPM;
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, time(NULL), time(NULL));
  int bpm = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_peek_current_value(metric)
    : 0;
  char buf[8];
  if (bpm > 0) {
    snprintf(buf, sizeof(buf), "%d", bpm);
  } else {
    snprintf(buf, sizeof(buf), "--");
  }
  int percent = CLAMP(bpm * 100 / 200, 0, 100);
  indicators_set_sw(buf, percent, GColorRed);
}

static void update_calories(void) {
  HealthMetric metric = HealthMetricActiveKCalories;
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, time_start_of_today(), time(NULL));
  int kcal = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(metric)
    : 0;
  char buf[8];
  snprintf(buf, sizeof(buf), "%d", kcal);
  int percent = CLAMP(kcal * 100 / 600, 0, 100);
  indicators_set_se(buf, percent, GColorYellow);
}

static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate || event == HealthEventSignificantUpdate) {
    update_step_count();
    update_calories();
  }
  if (event == HealthEventHeartRateUpdate || event == HealthEventSignificantUpdate) {
    update_heart_rate();
  }
}

static void window_load(Window *window) {
  window_set_background_color(window, g_color_background);

  Layer *root = window_get_root_layer(window);
  indicators_layer_create(root);
  date_layer_create(root);
  battery_layer_create(root);
  time_layer_create(root);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  time_layer_update(t);
  date_layer_update(t);

  BatteryChargeState bat = battery_state_service_peek();
  battery_layer_set(bat.charge_percent);
  char bat_buf[8];
  snprintf(bat_buf, sizeof(bat_buf), "%d%%", bat.charge_percent);
  indicators_set_ne(bat_buf, bat.charge_percent, GColorOrange);

  update_step_count();
  update_heart_rate();
  update_calories();
}

static void window_unload(Window *window) {
  date_layer_destroy();
  battery_layer_destroy();
  time_layer_destroy();
  indicators_layer_destroy();
}

static void init(void) {
  bool dark = persist_exists(MESSAGE_KEY_DarkTheme)
    ? persist_read_bool(MESSAGE_KEY_DarkTheme)
    : true;
  theme_apply(dark);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  health_service_events_subscribe(health_handler, NULL);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  health_service_events_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
