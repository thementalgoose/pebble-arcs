#pragma once
#include <pebble.h>

typedef enum {
  QUADRANT_NW    = 0,
  QUADRANT_NE    = 1,
  QUADRANT_SW    = 2,
  QUADRANT_SE    = 3,
  QUADRANT_COUNT = 4,
} Quadrant;

void indicators_layer_create(Layer *root);
void indicators_layer_destroy(void);
void indicators_layer_apply_theme(void);

// Bind label text, arc fill percent (0–100), and arc colour to a quadrant.
void indicators_set(Quadrant q, const char *label, int percent, GColor color);
