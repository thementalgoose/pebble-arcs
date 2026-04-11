#include <pebble.h>
#include "indicators.h"
#include "constants.h"

static Layer *s_indicators_layer;

static void indicators_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  // Inset by half the stroke width so the outer edge of the circle
  // sits flush against the edge of the display.
  uint16_t radius = ((bounds.size.w < bounds.size.h ? bounds.size.w : bounds.size.h) / 2) - (ARC_WIDTH / 2);
  GRect arc = GRect(center.x - radius, center.y - radius, radius * 2, radius * 2);

  int32_t arc_ne_start = DEG_TO_TRIGANGLE(ARC_NE_START);
  int32_t arc_ne_end = DEG_TO_TRIGANGLE(ARC_NE_END);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, ARC_WIDTH);
  graphics_draw_arc(ctx, arc, GOvalScaleModeFitCircle, arc_ne_start, arc_ne_end);

  int32_t arc_nw_start = DEG_TO_TRIGANGLE(ARC_NW_START);
  int32_t arc_nw_end = DEG_TO_TRIGANGLE(ARC_NW_END);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, ARC_WIDTH);
  graphics_draw_arc(ctx, arc, GOvalScaleModeFitCircle, arc_nw_start, arc_nw_end);

  int32_t arc_se_start = DEG_TO_TRIGANGLE(ARC_SE_START);
  int32_t arc_se_end = DEG_TO_TRIGANGLE(ARC_SE_END);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, ARC_WIDTH);
  graphics_draw_arc(ctx, arc, GOvalScaleModeFitCircle, arc_se_start, arc_se_end);

  int32_t arc_sw_start = DEG_TO_TRIGANGLE(ARC_SW_START);
  int32_t arc_sw_end = DEG_TO_TRIGANGLE(ARC_SW_END);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, ARC_WIDTH);
  graphics_draw_arc(ctx, arc, GOvalScaleModeFitCircle, arc_sw_start, arc_sw_end);
}

void indicators_layer_create(Layer *root) {
  GRect bounds = layer_get_bounds(root);
  s_bolt_path = gpath_create(&BOLT_PATH_INFO);
  s_indicators_layer = layer_create(bounds);
  layer_set_update_proc(s_indicators_layer, indicators_update_proc);
  layer_add_child(root, s_indicators_layer);
}

void indicators_layer_destroy(void) {
  layer_destroy(s_indicators_layer);
  gpath_destroy(s_bolt_path);
}
