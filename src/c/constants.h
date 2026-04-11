#pragma once

#define PEBBLE_ROUND            PBL_IF_ROUND_ELSE(true, false)
#define PEBBLE_LARGE            PBL_DISPLAY_WIDTH > 200

// ---------------------------------------------------------------------------
// Layout - All
// ---------------------------------------------------------------------------
#define EDGE_LEFT               2
#define EDGE_TOP                2
#define EDGE_BOTTOM             2
#define EDGE_RIGHT              2        
// Layout - Time
#define HOURS_FONT              fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS)
#define HOURS_HEIGHT            52
#define MINUTES_FONT            fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS)
#define MINUTES_HEIGHT          52
#define TIME_INSET              4
