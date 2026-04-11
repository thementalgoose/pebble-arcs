#include <pebble.h>
#include "indicators.h"
#include "constants.h"

#define TEXT_W  40
#define TEXT_H  16

static Layer *s_indicators_layer;

static char s_ne_text[8] = "12345";
static char s_nw_text[8] = "12345";
static char s_se_text[8] = "12345";
static char s_sw_text[8] = "12345";
static int  s_ne_pct = 30;
static int  s_nw_pct = 50;
static int  s_se_pct = 70;
static int  s_sw_pct = 90;

// Draws a background (dim) arc for the full span and a filled (white) arc up
// to `percent`, plus a text label at `text_rect`.  lo_deg/hi_deg are always
// the smaller/larger angle so the arc goes clockwise over the short path.
// When `reversed` is true the fill grows from hi toward lo (used for NE/SW
// so that all four arcs originate from the vertical centerline).
static void draw_arc(GContext *ctx, GRect arc_rect,
                     int lo_deg, int hi_deg,
                     const char *text, int percent,
                     bool reversed,
                     GRect text_rect) {
  int32_t angle_lo   = DEG_TO_TRIGANGLE(lo_deg);
  int32_t angle_hi   = DEG_TO_TRIGANGLE(hi_deg);
  int32_t angle_fill = reversed
    ? DEG_TO_TRIGANGLE(hi_deg - (hi_deg - lo_deg) * percent / 100)
    : DEG_TO_TRIGANGLE(lo_deg + (hi_deg - lo_deg) * percent / 100);

  graphics_context_set_stroke_width(ctx, ARC_WIDTH + 2);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_hi);

  graphics_context_set_stroke_width(ctx, ARC_WIDTH);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_hi);

  if (percent > 0) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    if (reversed) {
      graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_fill, angle_hi);
    } else {
      graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_fill);
    }
  }

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_14),
                     text_rect, GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);
}

static void draw_arc_ne(GContext *ctx, GRect arc_rect, GRect bounds,
                        const char *text, int percent) {
  GRect text_rect = GRect(bounds.size.w - TEXT_W - EDGE_RIGHT, EDGE_TOP, TEXT_W, TEXT_H);
  draw_arc(ctx, arc_rect,
           MIN(ARC_NE_START, ARC_NE_END), MAX(ARC_NE_START, ARC_NE_END),
           text, percent, true, text_rect);
}

static void draw_arc_nw(GContext *ctx, GRect arc_rect, GRect bounds,
                        const char *text, int percent) {
  GRect text_rect = GRect(EDGE_LEFT, EDGE_TOP, TEXT_W, TEXT_H);
  draw_arc(ctx, arc_rect,
           MIN(ARC_NW_START, ARC_NW_END), MAX(ARC_NW_START, ARC_NW_END),
           text, percent, false, text_rect);
}

static void draw_arc_se(GContext *ctx, GRect arc_rect, GRect bounds,
                        const char *text, int percent) {
  GRect text_rect = GRect(bounds.size.w - TEXT_W - EDGE_RIGHT,
                          bounds.size.h - TEXT_H - EDGE_BOTTOM, TEXT_W, TEXT_H);
  draw_arc(ctx, arc_rect,
           MIN(ARC_SE_START, ARC_SE_END), MAX(ARC_SE_START, ARC_SE_END),
           text, percent, false, text_rect);
}

static void draw_arc_sw(GContext *ctx, GRect arc_rect, GRect bounds,
                        const char *text, int percent) {
  GRect text_rect = GRect(EDGE_LEFT, bounds.size.h - TEXT_H - EDGE_BOTTOM, TEXT_W, TEXT_H);
  draw_arc(ctx, arc_rect,
           MIN(ARC_SW_START, ARC_SW_END), MAX(ARC_SW_START, ARC_SW_END),
           text, percent, true, text_rect);
}

static void indicators_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  uint16_t radius = (MIN(bounds.size.w, bounds.size.h) / 2) - (ARC_WIDTH / 2) - ARC_EDGE;
  GRect arc_rect = GRect(center.x - radius, center.y - radius, radius * 2, radius * 2);

  draw_arc_ne(ctx, arc_rect, bounds, s_ne_text, s_ne_pct);
  draw_arc_nw(ctx, arc_rect, bounds, s_nw_text, s_nw_pct);
  draw_arc_se(ctx, arc_rect, bounds, s_se_text, s_se_pct);
  draw_arc_sw(ctx, arc_rect, bounds, s_sw_text, s_sw_pct);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void indicators_set_ne(const char *text, int percent) {
  snprintf(s_ne_text, sizeof(s_ne_text), "%s", text);
  s_ne_pct = CLAMP(percent, 0, 100);
  if (s_indicators_layer) layer_mark_dirty(s_indicators_layer);
}

void indicators_set_nw(const char *text, int percent) {
  snprintf(s_nw_text, sizeof(s_nw_text), "%s", text);
  s_nw_pct = CLAMP(percent, 0, 100);
  if (s_indicators_layer) layer_mark_dirty(s_indicators_layer);
}

void indicators_set_se(const char *text, int percent) {
  snprintf(s_se_text, sizeof(s_se_text), "%s", text);
  s_se_pct = CLAMP(percent, 0, 100);
  if (s_indicators_layer) layer_mark_dirty(s_indicators_layer);
}

void indicators_set_sw(const char *text, int percent) {
  snprintf(s_sw_text, sizeof(s_sw_text), "%s", text);
  s_sw_pct = CLAMP(percent, 0, 100);
  if (s_indicators_layer) layer_mark_dirty(s_indicators_layer);
}

void indicators_layer_create(Layer *root) {
  GRect bounds = layer_get_bounds(root);
  s_indicators_layer = layer_create(bounds);
  layer_set_update_proc(s_indicators_layer, indicators_update_proc);
  layer_add_child(root, s_indicators_layer);
}

void indicators_layer_destroy(void) {
  layer_destroy(s_indicators_layer);
}
