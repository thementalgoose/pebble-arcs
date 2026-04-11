#include <pebble.h>
#include "battery.h"
#include "constants.h"
#include "date.h"
#include "indicators.h"
#include "time.h"

static Window *s_window;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  time_layer_update(tick_time);
  date_layer_update(tick_time);
}

static void battery_handler(BatteryChargeState state) {
  battery_layer_set(state.charge_percent);
}

static void window_load(Window *window) {
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
}

static void window_unload(Window *window) {
  date_layer_destroy();
  battery_layer_destroy();
  time_layer_destroy();
  indicators_layer_destroy();
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, BACKGROUND_COLOR);
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
