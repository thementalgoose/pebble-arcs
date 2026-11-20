#include <pebble.h>
#include <message_keys.auto.h>
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
    design_layer_apply_theme();
    indicators_layer_apply_theme();
  }
}

// ---------------------------------------------------------------------------
// App message
// ---------------------------------------------------------------------------

// The config page may send any field as CSTRING rather than INT depending on the control
// type (select, input). Parse whichever representation actually arrived.
static int tuple_int(const Tuple *t) {
  if (t->type == TUPLE_CSTRING) return atoi(t->value->cstring);
  switch (t->length) {
    case 1:  return (int)t->value->int8;
    case 2:  return (int)t->value->int16;
    default: return (int)t->value->int32;
  }
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

static int weather_update_interval(void) {
  return persist_exists(MESSAGE_KEY_WeatherUpdateInterval)
    ? persist_read_int(MESSAGE_KEY_WeatherUpdateInterval) : 60;
}

static bool weather_should_update(void) {
  return weather_update_interval() > 0 && quadrants_has_weather_metric();
}

static void weather_request_update(void) {
  if (!connection_service_peek_pebble_app_connection()) {
    return;
  }
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) == APP_MSG_OK) {
    dict_write_uint8(iter, MESSAGE_KEY_WeatherRequestUpdate, 1);
    app_message_outbox_send();
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *t;

  // Theme
  t = dict_find(iter, MESSAGE_KEY_DarkTheme);
  if (t) theme_apply(t->value->int32 != 0);

  // Quiet time indicator (right triangle)
  t = dict_find(iter, MESSAGE_KEY_QuietTimeIndicator);
  if (t) {
    persist_write_bool(MESSAGE_KEY_QuietTimeIndicator, t->value->int32 != 0);
    indicators_layer_apply_theme();
  }

  // Disconnection indicator (left triangle)
  t = dict_find(iter, MESSAGE_KEY_DisconnectIndicator);
  if (t) {
    persist_write_bool(MESSAGE_KEY_DisconnectIndicator, t->value->int32 != 0);
    indicators_layer_apply_theme();
  }

  // Outlined arc borders
  t = dict_find(iter, MESSAGE_KEY_OutlinedArcs);
  if (t) {
    bool outlined = t->value->int32 != 0;
    persist_write_bool(MESSAGE_KEY_OutlinedArcs, outlined);
  }

  // Show indicator text
  t = dict_find(iter, MESSAGE_KEY_ShowIndicatorText);
  if (t) {
    bool show = t->value->int32 != 0;
    persist_write_bool(MESSAGE_KEY_ShowIndicatorText, show);
    indicators_layer_apply_theme();
  }

  // Indicator width
  t = dict_find(iter, MESSAGE_KEY_IndicatorWidth);
  if (t) {
    indicators_set_width(tuple_int(t));
    design_layer_apply_theme();
  }

  // Quadrant options (config select fields arrive as CSTRING, e.g. "5\0\0\0")
  t = dict_find(iter, MESSAGE_KEY_TopLeft_Option);
  if (t) quadrants_set_option(QUADRANT_NW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_TopRight_Option);
  if (t) quadrants_set_option(QUADRANT_NE, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomLeft_Option);
  if (t) quadrants_set_option(QUADRANT_SW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomRight_Option);
  if (t) quadrants_set_option(QUADRANT_SE, tuple_int(t));

  // Quadrant arc colours (config page sends 24-bit RGB hex as int32, use GColorFromHEX to convert)
  t = dict_find(iter, MESSAGE_KEY_TopLeft_Colour);
  if (t) quadrants_set_color(QUADRANT_NW, GColorFromHEX(t->value->int32));

  t = dict_find(iter, MESSAGE_KEY_TopRight_Colour);
  if (t) quadrants_set_color(QUADRANT_NE, GColorFromHEX(t->value->int32));

  t = dict_find(iter, MESSAGE_KEY_BottomLeft_Colour);
  if (t) quadrants_set_color(QUADRANT_SW, GColorFromHEX(t->value->int32));

  t = dict_find(iter, MESSAGE_KEY_BottomRight_Colour);
  if (t) quadrants_set_color(QUADRANT_SE, GColorFromHEX(t->value->int32));

  // Quadrant range bounds
  t = dict_find(iter, MESSAGE_KEY_TopLeft_Min);
  if (t) quadrants_set_min(QUADRANT_NW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_TopLeft_Max);
  if (t) quadrants_set_max(QUADRANT_NW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_TopRight_Min);
  if (t) quadrants_set_min(QUADRANT_NE, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_TopRight_Max);
  if (t) quadrants_set_max(QUADRANT_NE, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomLeft_Min);
  if (t) quadrants_set_min(QUADRANT_SW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomLeft_Max);
  if (t) quadrants_set_max(QUADRANT_SW, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomRight_Min);
  if (t) quadrants_set_min(QUADRANT_SE, tuple_int(t));

  t = dict_find(iter, MESSAGE_KEY_BottomRight_Max);
  if (t) quadrants_set_max(QUADRANT_SE, tuple_int(t));

  // Weather configuration from config page
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

  // Phone battery from the plugin source (negative = unavailable)
  t = dict_find(iter, MESSAGE_KEY_PhoneBattery);
  if (t) metrics_set_phone_battery(tuple_signed_int(t));

  // Complication slots (percent < 0 = unsupported / no permission)
  {
    const uint32_t pct_keys[COMPLICATION_SLOTS] = {
      MESSAGE_KEY_Complication1_Percent, MESSAGE_KEY_Complication2_Percent,
      MESSAGE_KEY_Complication3_Percent, MESSAGE_KEY_Complication4_Percent,
      MESSAGE_KEY_Complication5_Percent };
    const uint32_t label_keys[COMPLICATION_SLOTS] = {
      MESSAGE_KEY_Complication1_Label, MESSAGE_KEY_Complication2_Label,
      MESSAGE_KEY_Complication3_Label, MESSAGE_KEY_Complication4_Label,
      MESSAGE_KEY_Complication5_Label };
    for (int s = 0; s < COMPLICATION_SLOTS; s++) {
      Tuple *pt = dict_find(iter, pct_keys[s]);
      Tuple *lt = dict_find(iter, label_keys[s]);
      if (pt && lt && lt->type == TUPLE_CSTRING) {
        metrics_set_complication(s + 1, tuple_signed_int(pt), lt->value->cstring);
      }
    }
  }

  // Animate indicator changes
  t = dict_find(iter, MESSAGE_KEY_AnimateIndicators);
  if (t) indicators_set_animated(t->value->int32 != 0);

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
  // Avoid re-rendering quadrants every minute — update them only on explicit
  // events (battery change, health events, config changes) to save power.
  // Request a weather update only when weather is actually displayed.
  int interval = weather_update_interval();
  if (tick_time->tm_min % interval == 0 && interval > 0 && weather_should_update()) {
    weather_request_update();
  }
}

static void battery_handler(BatteryChargeState state) {
  quadrants_render_all();
}

static void bluetooth_handler(bool connected) {
  design_layer_set_connected(connected);
  indicators_layer_apply_theme();
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
  time_layer_create(root);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  time_layer_update(t);
  date_layer_update(t);

  design_layer_set_connected(connection_service_peek_pebble_app_connection());
  quadrants_render_all();
}

static void window_unload(Window *window) {
  time_layer_destroy();
  date_layer_destroy();
  design_layer_destroy();
  indicators_layer_destroy();
}

// ---------------------------------------------------------------------------
// Init / deinit
// ---------------------------------------------------------------------------

static void init(void) {
  quadrants_load();

  weather_restore_cached_data();
  metrics_restore_cached_values();

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

  if (weather_should_update()) {
    weather_request_update();
  }

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
