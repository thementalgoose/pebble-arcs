#include <pebble.h>
#include <ctype.h>
#include "date.h"
#include "constants.h"

static void str_to_upper(char *s) {
  for (; *s; s++) { *s = toupper((unsigned char)*s); }
}

static TextLayer *s_dow_layer;   // day of week — above the time
static TextLayer *s_date_layer;  // "dd MMM"     — below the time
static char       s_dow_buf[12];
static char       s_date_buf[10];

void date_layer_create(Layer *root) {
  GRect bounds = layer_get_bounds(root);
  int16_t cx = bounds.size.h / 2;

  // The time block occupies [cx - HOURS_HEIGHT/2 .. cx + HOURS_HEIGHT/2].
  // Place the day-of-week immediately above and the date immediately below.
  GRect dow_frame  = GRect(0, cx - (HOURS_HEIGHT / 2) - WEEKDAY_HEIGHT + DATE_INSET + HOURS_MINS_Y_DELTA,
                           bounds.size.w, WEEKDAY_HEIGHT);
  GRect date_frame = GRect(0, cx + (HOURS_HEIGHT / 2) + DATE_INSET + HOURS_MINS_Y_DELTA,
                           bounds.size.w, DATE_HEIGHT);

  s_dow_layer = text_layer_create(dow_frame);
  text_layer_set_background_color(s_dow_layer, GColorClear);
  text_layer_set_text_color(s_dow_layer, INDICATOR_TEXT_COLOR);
  text_layer_set_font(s_dow_layer, WEEKDAY_FONT);
  text_layer_set_text_alignment(s_dow_layer, GTextAlignmentCenter);
  layer_add_child(root, text_layer_get_layer(s_dow_layer));

  s_date_layer = text_layer_create(date_frame);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, INDICATOR_TEXT_COLOR);
  text_layer_set_font(s_date_layer, DATE_FONT);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(root, text_layer_get_layer(s_date_layer));
}

void date_layer_update(struct tm *t) {
  strftime(s_dow_buf,  sizeof(s_dow_buf),  "%a",    t);
  strftime(s_date_buf, sizeof(s_date_buf), "%d %b", t);
  str_to_upper(s_dow_buf);
  str_to_upper(s_date_buf);
  text_layer_set_text(s_dow_layer,  s_dow_buf);
  text_layer_set_text(s_date_layer, s_date_buf);
}

void date_layer_apply_theme(void) {
  text_layer_set_text_color(s_dow_layer,  INDICATOR_TEXT_COLOR);
  text_layer_set_text_color(s_date_layer, INDICATOR_TEXT_COLOR);
}

void date_layer_destroy(void) {
  text_layer_destroy(s_dow_layer);
  text_layer_destroy(s_date_layer);
}
