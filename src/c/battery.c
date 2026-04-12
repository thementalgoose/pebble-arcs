#include <pebble.h>
#include "battery.h"
#include "constants.h"

static Layer *s_battery_layer;
static int    s_battery_pct = 100;

static void battery_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int fill_w = (s_battery_pct * bounds.size.w) / 100;
  
  graphics_context_set_fill_color(ctx, BAR_COLOR);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h / 2), 0, GCornerNone);

  graphics_context_set_fill_color(ctx, BAR_COLOR);
  graphics_fill_rect(ctx, GRect(0, 0, fill_w, bounds.size.h), 0, GCornerNone);
}

void battery_layer_create(Layer *root) {
  GRect bounds = layer_get_bounds(root);
  int16_t cx = bounds.size.h / 2;
  int16_t cy = bounds.size.w / 2;

  int16_t bar_y = cx + (HOURS_HEIGHT / 2) + DATE_INSET + HOURS_MINS_Y_DELTA
                  + DATE_HEIGHT + BATTERY_BAR_INSET;
  GRect frame = GRect(
    cy - (BATTERY_BAR_WIDTH / 2),
    bar_y, 
    BATTERY_BAR_WIDTH, 
    BATTERY_BAR_HEIGHT
  );
  s_battery_layer = layer_create(frame);
  layer_set_update_proc(s_battery_layer, battery_layer_update_proc);
  layer_add_child(root, s_battery_layer);
}

void battery_layer_set(int percent) {
  s_battery_pct = CLAMP(percent, 0, 100);
  if (s_battery_layer) layer_mark_dirty(s_battery_layer);
}

void battery_layer_set_visible(bool visible) {
  if (s_battery_layer) layer_set_hidden(s_battery_layer, !visible);
}

void battery_layer_apply_theme(void) {
  if (s_battery_layer) layer_mark_dirty(s_battery_layer);
}

void battery_layer_destroy(void) {
  layer_destroy(s_battery_layer);
}
