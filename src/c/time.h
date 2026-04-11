#pragma once
#include <pebble.h>

void time_layer_create(Layer *root);
void time_layer_update(struct tm *t);
void time_layer_apply_theme(void);
void time_layer_destroy(void);
