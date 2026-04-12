#include <pebble.h>
#include "time.h"
#include "constants.h"

static TextLayer *s_hours_layer;
static TextLayer *s_minutes_layer;
static TextLayer *s_colon_layer;
static char       s_hours_buf[3];
static char       s_minutes_buf[3];

void time_layer_create(Layer *root) {
  GRect bounds = layer_get_bounds(root);

  int16_t h = bounds.size.h;
  int16_t y_offset = (h / 2) - (HOURS_HEIGHT / 2) + HOURS_MINS_Y_DELTA;

  GRect hours_frame   = GRect(0, y_offset, (bounds.size.w / 2) - TIME_INSET, HOURS_HEIGHT);
  GRect minutes_frame = GRect((bounds.size.w / 2) + TIME_INSET, y_offset, bounds.size.w / 2, MINUTES_HEIGHT);
  GRect colon_frame   = GRect(0, y_offset, bounds.size.w, MINUTES_HEIGHT);

  s_hours_layer = text_layer_create(hours_frame);
  text_layer_set_background_color(s_hours_layer, GColorClear);
  text_layer_set_text_color(s_hours_layer, HOUR_TEXT_COLOR);
  text_layer_set_font(s_hours_layer, HOURS_FONT);
  text_layer_set_text_alignment(s_hours_layer, GTextAlignmentRight);
  layer_add_child(root, text_layer_get_layer(s_hours_layer));

  s_minutes_layer = text_layer_create(minutes_frame);
  text_layer_set_background_color(s_minutes_layer, GColorClear);
  text_layer_set_text_color(s_minutes_layer, MINUTE_TEXT_COLOR);
  text_layer_set_font(s_minutes_layer, MINUTES_FONT);
  text_layer_set_text_alignment(s_minutes_layer, GTextAlignmentLeft);
  layer_add_child(root, text_layer_get_layer(s_minutes_layer));

  s_colon_layer = text_layer_create(colon_frame);
  text_layer_set_background_color(s_colon_layer, GColorClear);
  text_layer_set_text_color(s_colon_layer, HOUR_TEXT_COLOR);
  text_layer_set_font(s_colon_layer, MINUTES_FONT);
  text_layer_set_text_alignment(s_colon_layer, GTextAlignmentCenter);
  text_layer_set_text(s_colon_layer, ":");
  layer_add_child(root, text_layer_get_layer(s_colon_layer));
}

void time_layer_update(struct tm *t) {
  bool is_24h = clock_is_24h_style();
  strftime(s_hours_buf,   sizeof(s_hours_buf),   is_24h ? "%H" : "%I", t);
  // Strip leading zero for 12h format (e.g. "01" -> "1")
  if (!is_24h && s_hours_buf[0] == '0') {
    memmove(s_hours_buf, s_hours_buf + 1, sizeof(s_hours_buf) - 1);
  }
  strftime(s_minutes_buf, sizeof(s_minutes_buf),  "%M", t);
  text_layer_set_text(s_hours_layer,   s_hours_buf);
  text_layer_set_text(s_minutes_layer, s_minutes_buf);
}

void time_layer_apply_theme(void) {
  text_layer_set_text_color(s_hours_layer,   HOUR_TEXT_COLOR);
  text_layer_set_text_color(s_minutes_layer, MINUTE_TEXT_COLOR);
  text_layer_set_text_color(s_colon_layer,   HOUR_TEXT_COLOR);
}

void time_layer_destroy(void) {
  text_layer_destroy(s_hours_layer);
  text_layer_destroy(s_minutes_layer);
  text_layer_destroy(s_colon_layer);
}
