#pragma once
#include <pebble.h>

void date_layer_create(Layer *root);
void date_layer_update(struct tm *t);
void date_layer_destroy(void);
