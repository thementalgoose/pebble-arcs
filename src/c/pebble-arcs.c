#include <pebble.h>
#include <message_keys.auto.h>
#include "battery.h"
#include "constants.h"
#include "date.h"
#include "design.h"
#include "indicators.h"
#include "metrics.h"
#include "quadrants.h"
#include "time.h"
#include "weather.h"

// ---------------------------------------------------------------------------
// Theme colour globals (declared extern in constants.h, used across modules)
// ---------------------------------------------------------------------------
GColor g_color_background;
GColor g_color_bar;
GColor g_color_hour;
GColor g_color_minute;
GColor g_color_indicator;

static Window *s_window;

// ---------------------------------------------------------------------------
// Theme
// ---------------------------------------------------------------------------

static void theme_apply(bool dark) {
  persist_write_bool(MESSAGE_KEY_DarkTheme, dark);
  if (dark) {
    g_color_background = GColorBlack;
    g_color_bar        = GColorWhite;
    g_color_hour       = GColorWhite;
    g_color_minute     = GColorLightGray;
    g_color_indicator  = GColorWhite;
  } else {
    g_color_background = GColorWhite;
    g_color_bar        = GColorBlack;
    g_color_hour       = GColorBlack;
    g_color_minute     = GColorDarkGray;
    g_color_indicator  = GColorBlack;
  }
  if (s_window && window_is_loaded(s_window)) {
    window_set_background_color(s_window, g_color_background);
    time_layer_apply_theme();
    date_layer_apply_theme();
    battery_layer_apply_theme();
    design_layer_apply_theme();
    indicators_layer_apply_theme();
  }
}

// ---------------------------------------------------------------------------
// Goals persistence helpers
// ---------------------------------------------------------------------------

static void goals_persist(int step_goal, int calorie_goal, int hr_lower, int hr_upper) {
  persist_write_int(MESSAGE_KEY_StepGoal,      step_goal);
  persist_write_int(MESSAGE_KEY_CalorieGoal,   calorie_goal);
  persist_write_int(MESSAGE_KEY_HeartRateLower, hr_lower);
  persist_write_int(MESSAGE_KEY_HeartRateUpper, hr_upper);
  metrics_set_goals(step_goal, calorie_goal, hr_lower, hr_upper);
}

static void goals_load(void) {
  metrics_set_goals(
    persist_exists(MESSAGE_KEY_StepGoal)       ? persist_read_int(MESSAGE_KEY_StepGoal)       : 5000,
    persist_exists(MESSAGE_KEY_CalorieGoal)    ? persist_read_int(MESSAGE_KEY_CalorieGoal)    : 2000,
    persist_exists(MESSAGE_KEY_HeartRateLower) ? persist_read_int(MESSAGE_KEY_HeartRateLower) : 40,
    persist_exists(MESSAGE_KEY_HeartRateUpper) ? persist_read_int(MESSAGE_KEY_HeartRateUpper) : 100
  );
  metrics_set_temperature_bounds(
    persist_exists(MESSAGE_KEY_TemperatureLower) ? persist_read_int(MESSAGE_KEY_TemperatureLower) : 5,
    persist_exists(MESSAGE_KEY_TemperatureUpper) ? persist_read_int(MESSAGE_KEY_TemperatureUpper) : 35
  );
  metrics_set_distance_goal(
    persist_exists(MESSAGE_KEY_DistanceGoal) ? persist_read_int(MESSAGE_KEY_DistanceGoal) : 5000
  );
}

// ---------------------------------------------------------------------------
// App message
// ---------------------------------------------------------------------------

// Clay may send any field as CSTRING rather than INT depending on the control
// type (select, input). Parse whichever representation actually arrived.
static int tuple_int(const Tuple *t) {
  return (t->type == TUPLE_CSTRING) ? atoi(t->value->cstring) : (int)t->value->int32;
}

// Signed variant: handles int8/int16/int32 lengths for values that can be
// negative (e.g. temperatures below 0). JS packs small integers as int8.
static int tuple_signed_int(const Tuple *t) {
  if (t->type == TUPLE_CSTRING) return atoi(t->value->cstring);
  switch (t->length) {
    case 1:  return (int)t->value->int8;
    case 2:  return (int)t->value->int16;
    default: return (int)t->value->int32;
  }
}

// ---------------------------------------------------------------------------
// Weather helpers
// ---------------------------------------------------------------------------

static void weather_request_update(void) {
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) == APP_MSG_OK) {
    dict_write_uint8(iter, MESSAGE_KEY_WeatherRequestUpdate, 1);
    app_message_outbox_send();
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *t;
  bool goals_changed = false;
  int step_goal    = persist_exists(MESSAGE_KEY_StepGoal)       ? persist_read_int(MESSAGE_KEY_StepGoal)       : 5000;
  int calorie_goal = persist_exists(MESSAGE_KEY_CalorieGoal)    ? persist_read_int(MESSAGE_KEY_CalorieGoal)    : 2000;
  int hr_lower     = persist_exists(MESSAGE_KEY_HeartRateLower) ? persist_read_int(MESSAGE_KEY_HeartRateLower) : 40;
  int hr_upper     = persist_exists(MESSAGE_KEY_HeartRateUpper) ? persist_read_int(MESSAGE_KEY_HeartRateUpper) : 100;

  // Theme
  t = dict_find(iter, MESSAGE_KEY_DarkTheme);
  if (t) theme_apply(t->value->int32 != 0);

  // Standalone battery indicator
  t = dict_find(iter, MESSAGE_KEY_BatteryIndicator);
  if (t) {
    bool show = t->value->int32 != 0;
    persist_write_bool(MESSAGE_KEY_BatteryIndicator, show);
    battery_layer_set_visible(show);
  }

  // Outlined arc borders
  t = dict_find(iter, MESSAGE_KEY_OutlinedArcs);
  if (t) {
    bool outlined = t->value->int32 != 0;
    persist_write_bool(MESSAGE_KEY_OutlinedArcs, outlined);
  }

  // Quadrant options (Clay select fields arrive as CSTRING, e.g. "5\0\0\0")
  t = dict_find(iter, MESSAGE_KEY_TopLeft_Option);
  if (t) quadrants_set_option(QUADRANT_NW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_TopRight_Option);
  if (t) quadrants_set_option(QUADRANT_NE, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomLeft_Option);
  if (t) quadrants_set_option(QUADRANT_SW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomRight_Option);
  if (t) quadrants_set_option(QUADRANT_SE, tuple_int(t));

  // Quadrant arc colours (Clay sends 24-bit RGB hex as int32, use GColorFromHEX to convert)
  t = dict_find(iter, MESSAGE_KEY_TopLeft_Colour);
  if (t) quadrants_set_color(QUADRANT_NW, GColorFromHEX(t->value->int32));

  t = dict_find(iter, MESSAGE_KEY_TopRight_Colour);
  if (t) quadrants_set_color(QUADRANT_NE, GColorFromHEX(t->value->int32));

  t = dict_find(iter, MESSAGE_KEY_BottomLeft_Colour);
  if (t) quadrants_set_color(QUADRANT_SW, GColorFromHEX(t->value->int32));

  t = dict_find(iter, MESSAGE_KEY_BottomRight_Colour);
  if (t) quadrants_set_color(QUADRANT_SE, GColorFromHEX(t->value->int32));

  // Goals (Clay input fields send strings)
  t = dict_find(iter, MESSAGE_KEY_StepGoal);
  if (t) { step_goal    = atoi(t->value->cstring); goals_changed = true; }

  t = dict_find(iter, MESSAGE_KEY_CalorieGoal);
  if (t) { calorie_goal = atoi(t->value->cstring); goals_changed = true; }

  t = dict_find(iter, MESSAGE_KEY_HeartRateLower);
  if (t) { hr_lower     = atoi(t->value->cstring); goals_changed = true; }

  t = dict_find(iter, MESSAGE_KEY_HeartRateUpper);
  if (t) { hr_upper     = atoi(t->value->cstring); goals_changed = true; }

  if (goals_changed) goals_persist(step_goal, calorie_goal, hr_lower, hr_upper);

  t = dict_find(iter, MESSAGE_KEY_TemperatureLower);
  if (t) {
    int val = atoi(t->value->cstring);
    persist_write_int(MESSAGE_KEY_TemperatureLower, val);
    metrics_set_temperature_bounds(
      val,
      persist_exists(MESSAGE_KEY_TemperatureUpper) ? persist_read_int(MESSAGE_KEY_TemperatureUpper) : 35
    );
  }

  t = dict_find(iter, MESSAGE_KEY_TemperatureUpper);
  if (t) {
    int val = atoi(t->value->cstring);
    persist_write_int(MESSAGE_KEY_TemperatureUpper, val);
    metrics_set_temperature_bounds(
      persist_exists(MESSAGE_KEY_TemperatureLower) ? persist_read_int(MESSAGE_KEY_TemperatureLower) : 5,
      val
    );
  }

  t = dict_find(iter, MESSAGE_KEY_DistanceGoal);
  if (t) {
    int val = atoi(t->value->cstring);
    persist_write_int(MESSAGE_KEY_DistanceGoal, val);
    metrics_set_distance_goal(val);
  }

  // Weather configuration from Clay
  t = dict_find(iter, MESSAGE_KEY_WeatherUseCelsius);
  if (t) {
    bool celsius = t->value->int32 != 0;
    persist_write_bool(MESSAGE_KEY_WeatherUseCelsius, celsius);
    weather_set_use_celsius(celsius);
  }

  t = dict_find(iter, MESSAGE_KEY_WeatherUpdateInterval);
  if (t) persist_write_int(MESSAGE_KEY_WeatherUpdateInterval, tuple_int(t));

  // Weather data from phone
  t = dict_find(iter, MESSAGE_KEY_WeatherTemperature);
  if (t) weather_set_temperature(tuple_signed_int(t));

  t = dict_find(iter, MESSAGE_KEY_WeatherCondition);
  if (t) weather_set_condition((int)t->value->int32);

  quadrants_render_all();
}

// ---------------------------------------------------------------------------
// Event handlers
// ---------------------------------------------------------------------------

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  time_layer_update(tick_time);
  date_layer_update(tick_time);
  quadrants_render_all();

  // Request a weather update at the configured interval
  int interval = persist_exists(MESSAGE_KEY_WeatherUpdateInterval)
    ? persist_read_int(MESSAGE_KEY_WeatherUpdateInterval) : 30;
  if (interval > 0 && tick_time->tm_min % interval == 0) {
    weather_request_update();
  }
}

static void battery_handler(BatteryChargeState state) {
  battery_layer_set(state.charge_percent);
  quadrants_render_all();
}

static void bluetooth_handler(bool connected) {
  design_layer_set_connected(connected);
}

static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate  ||
      event == HealthEventHeartRateUpdate ||
      event == HealthEventSignificantUpdate) {
    quadrants_render_all();
  }
}

// ---------------------------------------------------------------------------
// Window
// ---------------------------------------------------------------------------

static void window_load(Window *window) {
  window_set_background_color(window, g_color_background);

  Layer *root = window_get_root_layer(window);
  design_layer_create(root);
  indicators_layer_create(root);
  date_layer_create(root);
  battery_layer_create(root);
  time_layer_create(root);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  time_layer_update(t);
  date_layer_update(t);

  battery_layer_set(battery_state_service_peek().charge_percent);
  battery_layer_set_visible(
    persist_exists(MESSAGE_KEY_BatteryIndicator)
      ? persist_read_bool(MESSAGE_KEY_BatteryIndicator) : true
  );
  design_layer_set_connected(connection_service_peek_pebble_app_connection());
  quadrants_render_all();
}

static void window_unload(Window *window) {
  time_layer_destroy();
  date_layer_destroy();
  battery_layer_destroy();
  design_layer_destroy();
  indicators_layer_destroy();
}

// ---------------------------------------------------------------------------
// Init / deinit
// ---------------------------------------------------------------------------

static void init(void) {
  quadrants_load();
  goals_load();

  // Restore persisted weather unit preference
  if (persist_exists(MESSAGE_KEY_WeatherUseCelsius)) {
    weather_set_use_celsius(persist_read_bool(MESSAGE_KEY_WeatherUseCelsius));
  }

  bool dark = persist_exists(MESSAGE_KEY_DarkTheme)
    ? persist_read_bool(MESSAGE_KEY_DarkTheme) : true;
  theme_apply(dark);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  health_service_events_subscribe(health_handler, NULL);
  connection_service_subscribe((ConnectionHandlers){ .pebble_app_connection_handler = bluetooth_handler });
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  health_service_events_unsubscribe();
  connection_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
