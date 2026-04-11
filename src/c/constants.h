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
#define HOURS_HEIGHT            42
#define MINUTES_FONT            fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS)
#define MINUTES_HEIGHT          42
#define TIME_INSET              4
#define HOURS_MINS_Y_DELTA      -8

// Layout - Date
#define WEEKDAY_FONT            fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)
#define WEEKDAY_HEIGHT          14
#define DATE_FONT               fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)
#define DATE_HEIGHT             18
#define DATE_INSET              4

// ---------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------
#ifndef MIN
#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#endif
#ifndef CLAMP
#define CLAMP(x, lo, hi)    MIN(MAX((x), (lo)), (hi))
#endif

// ---------------------------------------------------------------------------
// Layout - Complications
// ---------------------------------------------------------------------------
#define ARC_SE_START            110
#define ARC_SE_END              160
#define ARC_SW_START            200
#define ARC_SW_END              250
#define ARC_NE_START            20
#define ARC_NE_END              70
#define ARC_NW_START            290
#define ARC_NW_END              340
// 
#define ARC_WIDTH               PBL_IF_ROUND_ELSE(8, 8)
#define ARC_EDGE                2