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
#define ARC_WIDTH               PBL_IF_ROUND_ELSE(14, 8)
// 
#define EDGE_NW                 {{8, 48}, {8, 8}, {48, 8}}