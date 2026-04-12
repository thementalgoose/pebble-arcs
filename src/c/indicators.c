#include <pebble.h>
#include "indicators.h"
#include "constants.h"

static Layer *s_layer;
static char   s_text[QUADRANT_COUNT][8];
static int    s_pct[QUADRANT_COUNT];
static GColor s_color[QUADRANT_COUNT];

// ---------------------------------------------------------------------------
// Arc drawing helpers
// ---------------------------------------------------------------------------

// Returns a text rect centred just inside the arc band at the arc's midpoint angle.
// Used on round displays to place labels within the arc rather than in screen corners.
static GRect text_rect_for_arc(GPoint center, uint16_t radius, int lo_deg, int hi_deg) {
  int     mid_deg     = (lo_deg + hi_deg) / 2;
  int32_t angle       = DEG_TO_TRIGANGLE(mid_deg);
  uint16_t text_r     = radius - (ARC_WIDTH / 2) - (ARC_BORDER / 2) - (TEXT_H / 2) - INDICATOR_TEXT_INSET;
  int16_t x = center.x + (int16_t)(sin_lookup(angle) * (int32_t)text_r / TRIG_MAX_RATIO);
  int16_t y = center.y - (int16_t)(cos_lookup(angle) * (int32_t)text_r / TRIG_MAX_RATIO) - 3;
  return GRect(x - TEXT_W / 2, y - TEXT_H / 2, TEXT_W, TEXT_H);
}

// Draws a dim background arc for the full span, then a coloured fill arc up to
// `percent`, plus a text label. `reversed` makes the fill grow from the high
// end toward the low end (used for NE/SW so arcs originate from the vertical
// centre-line on both sides).
static void draw_arc(GContext *ctx, GRect arc_rect,
                     int lo_deg, int hi_deg,
                     const char *text, int percent,
                     GColor color, bool reversed,
                     GRect text_rect) {
  int32_t angle_lo   = DEG_TO_TRIGANGLE(lo_deg);
  int32_t angle_hi   = DEG_TO_TRIGANGLE(hi_deg);
  int32_t angle_fill = reversed
    ? DEG_TO_TRIGANGLE(hi_deg - (hi_deg - lo_deg) * percent / 100)
    : DEG_TO_TRIGANGLE(lo_deg + (hi_deg - lo_deg) * percent / 100);

  graphics_context_set_stroke_width(ctx, ARC_WIDTH + ARC_BORDER);
  graphics_context_set_stroke_color(ctx, BAR_COLOR);
  graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_hi);

  graphics_context_set_stroke_width(ctx, ARC_WIDTH);
  graphics_context_set_stroke_color(ctx, BACKGROUND_COLOR);
  graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_hi);

  if (percent > 0) {
    graphics_context_set_stroke_color(ctx, color);
    if (reversed) {
      graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_fill, angle_hi);
    } else {
      graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_fill);
    }
  }

  graphics_context_set_text_color(ctx, INDICATOR_TEXT_COLOR);
  graphics_draw_text(ctx, text, INDICATOR_FONT,
                     text_rect, GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);
}

static void draw_quadrant(GContext *ctx, GRect arc_rect, GRect bounds,
                          GPoint center, uint16_t radius, Quadrant q) {
  const char *text    = s_text[q];
  int         percent = s_pct[q];
  GColor      color   = s_color[q];

  switch (q) {
    case QUADRANT_NW: {
      int lo = MIN(ARC_NW_START, ARC_NW_END), hi = MAX(ARC_NW_START, ARC_NW_END);
      draw_arc(
        ctx, 
        arc_rect, 
        lo,
        hi, 
        text, 
        percent, 
        color, 
        /*reversed=*/false,
        PBL_IF_ROUND_ELSE(
          text_rect_for_arc(center, radius, lo, hi),
          GRect(EDGE_LEFT, EDGE_TOP, TEXT_W, TEXT_H)
        )
      );
      break;
    }
    case QUADRANT_NE: {
      int lo = MIN(ARC_NE_START, ARC_NE_END), hi = MAX(ARC_NE_START, ARC_NE_END);
      draw_arc(
        ctx, 
        arc_rect, 
        lo,
        hi, 
        text, 
        percent, 
        color, 
        /*reversed=*/true,
        PBL_IF_ROUND_ELSE(
          text_rect_for_arc(center, radius, lo, hi),
          GRect(bounds.size.w - TEXT_W - EDGE_RIGHT, EDGE_TOP, TEXT_W, TEXT_H)
        )
      );
      break;
    }
    case QUADRANT_SW: {
      int lo = MIN(ARC_SW_START, ARC_SW_END), hi = MAX(ARC_SW_START, ARC_SW_END);
      draw_arc(
        ctx, 
        arc_rect, 
        lo,
        hi, 
        text, 
        percent, 
        color, 
        /*reversed=*/true,
        PBL_IF_ROUND_ELSE(
          text_rect_for_arc(center, radius, lo, hi),
          GRect(EDGE_LEFT, bounds.size.h - (TEXT_H + EDGE_BOTTOM + 8), TEXT_W, TEXT_H)
        )
      );
      break;
    }
    case QUADRANT_SE: {
      int lo = MIN(ARC_SE_START, ARC_SE_END), hi = MAX(ARC_SE_START, ARC_SE_END);
      draw_arc(
        ctx, 
        arc_rect, 
        lo,
        hi, 
        text, 
        percent, 
        color, 
        /*reversed=*/false,
        PBL_IF_ROUND_ELSE(
          text_rect_for_arc(center, radius, lo, hi),
          GRect(bounds.size.w - TEXT_W - EDGE_RIGHT,
                bounds.size.h - (TEXT_H + EDGE_BOTTOM + 8), TEXT_W, TEXT_H)
        )
      );
      break;
    }
    default:
      break;
  }
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect    bounds = layer_get_bounds(layer);
  GPoint   center = grect_center_point(&bounds);
  uint16_t radius = (MIN(bounds.size.w, bounds.size.h) / 2) - (ARC_WIDTH / 2) - ARC_EDGE;
  GRect    arc_rect = GRect(center.x - radius, center.y - radius, radius * 2, radius * 2);

  for (int q = 0; q < QUADRANT_COUNT; q++) {
    draw_quadrant(ctx, arc_rect, bounds, center, radius, (Quadrant)q);
  }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void indicators_set(Quadrant q, const char *label, int percent, GColor color) {
  snprintf(s_text[q], sizeof(s_text[q]), "%s", label);
  s_pct[q]   = CLAMP(percent, 0, 100);
  s_color[q] = color;
  if (s_layer) layer_mark_dirty(s_layer);
}

void indicators_layer_create(Layer *root) {
  GRect bounds = layer_get_bounds(root);
  for (int q = 0; q < QUADRANT_COUNT; q++) {
    s_text[q][0] = '\0';
    s_pct[q]     = 0;
    s_color[q]   = BAR_COLOR;
  }
  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, layer_update_proc);
  layer_add_child(root, s_layer);
}

void indicators_layer_apply_theme(void) {
  if (s_layer) layer_mark_dirty(s_layer);
}

void indicators_layer_destroy(void) {
  layer_destroy(s_layer);
  s_layer = NULL;
}
