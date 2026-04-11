#pragma once
#include <pebble.h>

void indicators_layer_create(Layer *root);
void indicators_layer_apply_theme(void);
void indicators_layer_destroy(void);

void indicators_set_ne(const char *text, int percent, GColor color);
void indicators_set_nw(const char *text, int percent, GColor color);
void indicators_set_se(const char *text, int percent, GColor color);
void indicators_set_sw(const char *text, int percent, GColor color);
