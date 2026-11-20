#include <pebble.h>
#include "design.h"
#include "indicators.h"
#include "constants.h"

static Layer *s_design_layer;
static bool   s_connected = true;

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

static void draw_dot(GContext *ctx, GPoint center, int16_t rx, int16_t ry, int angle_deg) {
  int32_t angle = DEG_TO_TRIGANGLE(angle_deg);
  GPoint p = {
    .x = center.x + (int16_t)(sin_lookup(angle) * (int32_t)rx / TRIG_MAX_RATIO),
    .y = center.y - (int16_t)(cos_lookup(angle) * (int32_t)ry / TRIG_MAX_RATIO),
  };
  graphics_fill_circle(ctx, p, 1);
}

// Draws 3 dots (ellipsis) along the arc at the given center angle.
// Dots are spaced 6° apart, centred on `center_deg`.
static void draw_ellipsis(GContext *ctx, GPoint center, int16_t rx, int16_t ry, int center_deg) {
  draw_dot(ctx, center, rx, ry, center_deg - 4);
  draw_dot(ctx, center, rx, ry - 1, center_deg);
  draw_dot(ctx, center, rx, ry, center_deg + 4);
}

static void design_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect   bounds   = layer_get_bounds(layer);
  Window *win      = layer_get_window(layer);
  int16_t full_h   = win ? layer_get_bounds(window_get_root_layer(win)).size.h : bounds.size.h;
  GPoint  center   = grect_center_point(&bounds);
  int     arc_w    = indicators_get_width();
  int     inset    = (arc_w / 2) + ARC_EDGE;
  int16_t rx       = MAX(1, (bounds.size.w / 2) - inset);
  int16_t v_pad    = (bounds.size.h < full_h) ? ((arc_w / 2) + 2) : MAX(inset, (full_h / 2) - rx);
  int16_t top_y    = v_pad;
  int16_t bottom_y = MAX(top_y + 2, bounds.size.h - v_pad);
  int16_t ry       = (bottom_y - top_y) / 2;
  GPoint  arc_ctr  = GPoint(center.x, top_y + ry);

  graphics_context_set_fill_color(ctx, BAR_COLOR);

  // if (s_connected) draw_ellipsis(ctx, arc_ctr, rx, ry,   0);  // top
  // if (s_connected) draw_ellipsis(ctx, arc_ctr, rx, ry,  90);  // right
  if (s_connected) draw_ellipsis(ctx, arc_ctr, rx, ry, 180);  // bottom
  // if (s_connected) draw_ellipsis(ctx, arc_ctr, rx, ry, 270);  // left
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void design_layer_create(Layer *root) {
  GRect bounds = layer_get_unobstructed_bounds(root);
  s_design_layer = layer_create(bounds);
  layer_set_update_proc(s_design_layer, design_layer_update_proc);
  layer_add_child(root, s_design_layer);
}

void design_layer_resize(GRect bounds) {
  if (s_design_layer) {
    layer_set_frame(s_design_layer, bounds);
    layer_mark_dirty(s_design_layer);
  }
}

void design_layer_set_connected(bool connected) {
  s_connected = connected;
  if (s_design_layer) layer_mark_dirty(s_design_layer);
}

void design_layer_apply_theme(void) {
  if (s_design_layer) layer_mark_dirty(s_design_layer);
}

void design_layer_destroy(void) {
  layer_destroy(s_design_layer);
  s_design_layer = NULL;
}
