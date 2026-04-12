#pragma once
#include <pebble.h>
#include "indicators.h"

// Load persisted quadrant config (options and colours) from storage.
// Call once during app init before the window is created.
void quadrants_load(void);

// Render all four quadrants using current options, colours, and live metric data.
void quadrants_render_all(void);

// Update the option shown in a quadrant and persist the change.
void quadrants_set_option(Quadrant q, int option);

// Update the arc fill colour for a quadrant and persist the change.
// Pass GColorClear to fall back to the metric's default colour.
void quadrants_set_color(Quadrant q, GColor color);
