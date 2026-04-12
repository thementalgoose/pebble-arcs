#include <pebble.h>
#include "design.h"
#include "constants.h"

static Layer *s_design_layer;
static bool   s_connected = true;

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

static void draw_dot(GContext *ctx, GPoint center, uint16_t radius, int angle_deg) {
  int32_t angle = DEG_TO_TRIGANGLE(angle_deg);
  GPoint p = {
    .x = center.x + (int16_t)(sin_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO),
    .y = center.y - (int16_t)(cos_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO),
  };
  graphics_fill_circle(ctx, p, 1);
}

// Draws 3 dots (ellipsis) along the arc at the given center angle.
// Dots are spaced 6° apart, centred on `center_deg`.
static void draw_ellipsis(GContext *ctx, GPoint center, uint16_t radius, int center_deg) {
  draw_dot(ctx, center, radius, center_deg - 4);
  draw_dot(ctx, center, radius - 1, center_deg);
  draw_dot(ctx, center, radius, center_deg + 4);
}

static void design_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect    bounds = layer_get_bounds(layer);
  GPoint   center = grect_center_point(&bounds);
  uint16_t radius = (MIN(bounds.size.w, bounds.size.h) / 2) - (ARC_WIDTH / 2) - ARC_EDGE;

  graphics_context_set_fill_color(ctx, BAR_COLOR);

  // draw_ellipsis(ctx, center, radius,   0);  // top
  // draw_ellipsis(ctx, center, radius,  90);  // right
  if (s_connected) draw_ellipsis(ctx, center, radius, 180);  // bottom
  // draw_ellipsis(ctx, center, radius, 270);  // left
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void design_layer_create(Layer *root) {
  GRect bounds = layer_get_bounds(root);
  s_design_layer = layer_create(bounds);
  layer_set_update_proc(s_design_layer, design_layer_update_proc);
  layer_add_child(root, s_design_layer);
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
