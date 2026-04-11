#pragma once
#include <pebble.h>

void battery_layer_create(Layer *root);
void battery_layer_set(int percent);
void battery_layer_apply_theme(void);
void battery_layer_destroy(void);
