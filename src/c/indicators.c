#include <pebble.h>
#include <message_keys.auto.h>
#include "indicators.h"
#include "constants.h"
#include <string.h>

static Layer *s_layer;
static char   s_text[QUADRANT_COUNT][8];
static int    s_pct[QUADRANT_COUNT];
static GColor s_color[QUADRANT_COUNT];
static int    s_indicator_width = -1;

int indicators_get_width(void) {
  if (s_indicator_width < 0) {
    s_indicator_width = persist_exists(MESSAGE_KEY_IndicatorWidth)
      ? persist_read_int(MESSAGE_KEY_IndicatorWidth)
      : DEFAULT_INDICATOR_WIDTH;
    if (s_indicator_width < 1 || s_indicator_width >= 25) {
      s_indicator_width = DEFAULT_INDICATOR_WIDTH;
      persist_write_int(MESSAGE_KEY_IndicatorWidth, s_indicator_width);
    }
  }
  return s_indicator_width;
}

int indicators_get_border(void) {
  int width = indicators_get_width();
  int border = (width * ARC_BORDER + DEFAULT_INDICATOR_WIDTH / 2) / DEFAULT_INDICATOR_WIDTH;
  return (border < 1) ? 1 : border;
}

void indicators_set_width(int width) {
  if (width < 1 || width >= 25) {
    width = DEFAULT_INDICATOR_WIDTH;
  }
  if (s_indicator_width != width) {
    s_indicator_width = width;
    persist_write_int(MESSAGE_KEY_IndicatorWidth, width);
    if (s_layer) layer_mark_dirty(s_layer);
  }
}

static bool outlined_arcs_enabled(void) {
  return persist_exists(MESSAGE_KEY_OutlinedArcs)
    ? persist_read_bool(MESSAGE_KEY_OutlinedArcs)
    : (PBL_IF_COLOR_ELSE(false, true));
}

static GColor fade_color(GColor color) {
  #if PBL_COLOR
  static const GColor k_light_background_colors[] = {
    GColorCeleste,
    GColorLightGray,
    GColorPastelYellow,
    GColorMintGreen,
    GColorInchworm,
    GColorIcterine,
    GColorBabyBlueEyes,
    GColorMintGreen,
    GColorRichBrilliantLavender,
    GColorMelon,
  };

  static const GColor k_dark_background_colors[] = {
    GColorDarkGray,
    GColorOxfordBlue,
    GColorImperialPurple,
    GColorDarkGreen,
    GColorMidnightGreen,
    GColorBulgarianRose,
    GColorWindsorTan
  };

  const bool dark_theme = BACKGROUND_COLOR.argb == GColorBlack.argb;
  const GColor *palette = dark_theme
    ? k_dark_background_colors : k_light_background_colors;
  size_t palette_count = dark_theme
    ? (sizeof(k_dark_background_colors) / sizeof(k_dark_background_colors[0]))
    : (sizeof(k_light_background_colors) / sizeof(k_light_background_colors[0]));

  uint8_t target_r = color.r / (dark_theme ? 1.5 : 0.5);
  uint8_t target_g = color.g / (dark_theme ? 1.5 : 0.5);
  uint8_t target_b = color.b / (dark_theme ? 1.5 : 0.5);

  GColor best_color = palette[0];
  uint32_t best_distance = UINT32_MAX;

  for (size_t i = 0; i < palette_count; i++) {
    GColor candidate = palette[i];
    uint32_t dr = (target_r > candidate.r) ? (target_r - candidate.r) : (candidate.r - target_r);
    uint32_t dg = (target_g > candidate.g) ? (target_g - candidate.g) : (candidate.g - target_g);
    uint32_t db = (target_b > candidate.b) ? (target_b - candidate.b) : (candidate.b - target_b);
    uint32_t distance = dr * dr + dg * dg + db * db;
    if (distance < best_distance) {
      best_distance = distance;
      best_color = candidate;
    }
  }

  return best_color;
#else
  return color;
#endif
}

// ---------------------------------------------------------------------------
// Arc drawing helpers
// ---------------------------------------------------------------------------

#if PBL_ROUND
// Returns a text rect positioned along the arc band for round displays.
static GRect text_rect_for_arc(GPoint center, uint16_t radius, int lo_deg, int hi_deg, Quadrant q) {
  int span = hi_deg - lo_deg;
  int mid_deg = (lo_deg + hi_deg) / 2;
  int shift = (span * 20) / 100;
  int target_deg = mid_deg;

  switch (q) {
    case QUADRANT_NW: target_deg = mid_deg + shift; break;
    case QUADRANT_NE: target_deg = mid_deg - shift; break;
    case QUADRANT_SW: target_deg = mid_deg - shift; break;
    case QUADRANT_SE: target_deg = mid_deg + shift; break;
    default: break;
  }

  int32_t angle       = DEG_TO_TRIGANGLE(target_deg);
  int     arc_w       = indicators_get_width();
  int     arc_b       = indicators_get_border();
  uint16_t text_r     = radius - (arc_w / 2) - (arc_b / 2) - (TEXT_H / 2) - INDICATOR_TEXT_INSET;
  int16_t x = center.x + (int16_t)(sin_lookup(angle) * (int32_t)text_r / TRIG_MAX_RATIO);
  int16_t y = center.y - (int16_t)(cos_lookup(angle) * (int32_t)text_r / TRIG_MAX_RATIO) - 3;
  switch (q) {
    case QUADRANT_NW: x += INNER_TEXT_PADDING; y += INNER_TEXT_PADDING; break;
    case QUADRANT_NE: x -= INNER_TEXT_PADDING; y += INNER_TEXT_PADDING; break;
    case QUADRANT_SW: x += INNER_TEXT_PADDING; y -= INNER_TEXT_PADDING; break;
    case QUADRANT_SE: x -= INNER_TEXT_PADDING; y -= INNER_TEXT_PADDING; break;
    default: break;
  }
  return GRect(x - TEXT_W / 2, y - TEXT_H / 2, TEXT_W, TEXT_H);
}
#endif

// Draws a dim background arc for the full span, then a coloured fill arc up to
// `percent`, plus a text label. `reversed` makes the fill grow from the high
// end toward the low end (used for NE/SW so arcs originate from the vertical
// centre-line on both sides).
static void draw_arc(GContext *ctx, GRect arc_rect,
                     int lo_deg, int hi_deg,
                     const char *text, int percent,
                     GColor color, bool reversed,
                     GRect text_rect, GTextAlignment text_alignment) {
  int32_t angle_lo   = DEG_TO_TRIGANGLE(lo_deg);
  int32_t angle_hi   = DEG_TO_TRIGANGLE(hi_deg);
  int32_t angle_fill = reversed
    ? DEG_TO_TRIGANGLE(hi_deg - (hi_deg - lo_deg) * percent / 100)
    : DEG_TO_TRIGANGLE(lo_deg + (hi_deg - lo_deg) * percent / 100);

  int arc_w = indicators_get_width();
  int arc_b = indicators_get_border();

  if (outlined_arcs_enabled()) {
    graphics_context_set_stroke_width(ctx, arc_w + arc_b);
    graphics_context_set_stroke_color(ctx, BAR_COLOR);
    graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_hi);

    graphics_context_set_stroke_width(ctx, arc_w);
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
  } else {
    graphics_context_set_stroke_width(ctx, arc_w);
    graphics_context_set_stroke_color(ctx, fade_color(color));
    graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_hi);

    if (percent > 0) {
      graphics_context_set_stroke_color(ctx, color);
      if (reversed) {
        graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_fill, angle_hi);
      } else {
        graphics_draw_arc(ctx, arc_rect, GOvalScaleModeFitCircle, angle_lo, angle_fill);
      }
    }
  }

  graphics_context_set_text_color(ctx, INDICATOR_TEXT_COLOR);
  graphics_draw_text(ctx, text, INDICATOR_FONT,
                     text_rect, GTextOverflowModeTrailingEllipsis,
                     text_alignment, NULL);
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
          text_rect_for_arc(center, radius, lo, hi, q),
          GRect(EDGE_LEFT + INNER_TEXT_PADDING, EDGE_TOP + INDICATOR_Y_OFFSET + INNER_TEXT_PADDING, TEXT_W, TEXT_H)
        ),
        GTextAlignmentLeft
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
          text_rect_for_arc(center, radius, lo, hi, q),
          GRect(bounds.size.w - TEXT_W - EDGE_RIGHT - INNER_TEXT_PADDING, EDGE_TOP + INDICATOR_Y_OFFSET + INNER_TEXT_PADDING, TEXT_W, TEXT_H)
        ),
        GTextAlignmentRight
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
          text_rect_for_arc(center, radius, lo, hi, q),
          GRect(EDGE_LEFT + INNER_TEXT_PADDING, bounds.size.h - TEXT_H - EDGE_BOTTOM - INNER_TEXT_PADDING, TEXT_W, TEXT_H)
        ),
        GTextAlignmentLeft
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
          text_rect_for_arc(center, radius, lo, hi, q),
          GRect(bounds.size.w - TEXT_W - EDGE_RIGHT - INNER_TEXT_PADDING,
                bounds.size.h - TEXT_H - EDGE_BOTTOM - INNER_TEXT_PADDING, TEXT_W, TEXT_H)
        ),
        GTextAlignmentRight
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
  int      arc_w  = indicators_get_width();
  uint16_t radius = (MIN(bounds.size.w, bounds.size.h) / 2) - (arc_w / 2) - ARC_EDGE;
  GRect    arc_rect = GRect(center.x - radius, center.y - radius, radius * 2, radius * 2);

  for (int q = 0; q < QUADRANT_COUNT; q++) {
    draw_quadrant(ctx, arc_rect, bounds, center, radius, (Quadrant)q);
  }

  // Right triangle: Quiet time indicator (when quiet time is active and enabled)
  bool quiet_enabled = persist_exists(MESSAGE_KEY_QuietTimeIndicator)
    ? persist_read_bool(MESSAGE_KEY_QuietTimeIndicator) : true;
  bool quiet_active = quiet_time_is_active() && quiet_enabled;
  if (quiet_active) {
    graphics_context_set_fill_color(ctx, INDICATOR_TEXT_COLOR);
    int16_t center_y = bounds.size.h / 2;
    int16_t tri_h = PBL_IF_ROUND_ELSE(12, 10);
    int16_t tri_w = (tri_h * 3) / 4;

    // Right triangle: tip at the right screen edge, base points inward
    GPoint right_points[3] = {
      {-tri_w, 0},
      {0, -tri_h},
      {0, tri_h},
    };
    GPathInfo right_info = { .num_points = 3, .points = right_points };
    GPath *right_path = gpath_create(&right_info);
    gpath_move_to(right_path, GPoint(bounds.size.w - 1, center_y));
    gpath_draw_filled(ctx, right_path);
    gpath_destroy(right_path);
  }

  // Left triangle: Disconnection indicator (when disconnected and enabled)
  bool disconnect_enabled = persist_exists(MESSAGE_KEY_DisconnectIndicator)
    ? persist_read_bool(MESSAGE_KEY_DisconnectIndicator) : true;
  bool connected = connection_service_peek_pebble_app_connection();
  if (!connected && disconnect_enabled) {
    graphics_context_set_fill_color(ctx, INDICATOR_TEXT_COLOR);
    int16_t center_y = bounds.size.h / 2;
    int16_t tri_h = PBL_IF_ROUND_ELSE(12, 10);
    int16_t tri_w = (tri_h * 3) / 4;
    GPoint left_points[3] = {
      {tri_w, 0},
      {0, -tri_h},
      {0, tri_h},
    };
    GPathInfo left_info = { .num_points = 3, .points = left_points };
    GPath *left_path = gpath_create(&left_info);
    gpath_move_to(left_path, GPoint(0, center_y));
    gpath_draw_filled(ctx, left_path);
    gpath_destroy(left_path);
  }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void indicators_set(Quadrant q, const char *label, int percent, GColor color) {
  char new_text[sizeof(s_text[q])];
  snprintf(new_text, sizeof(new_text), "%s", label);
  int new_pct = CLAMP(percent, 0, 100);
  GColor new_color = color;

  bool changed = (strcmp(s_text[q], new_text) != 0) || (s_pct[q] != new_pct) || (s_color[q].argb != new_color.argb);
  if (changed) {
    snprintf(s_text[q], sizeof(s_text[q]), "%s", new_text);
    s_pct[q] = new_pct;
    s_color[q] = new_color;
    if (s_layer) layer_mark_dirty(s_layer);
  }
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
