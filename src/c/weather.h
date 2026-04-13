#pragma once
#include <pebble.h>

// Returns true once at least one weather update has been received.
bool weather_has_data(void);

// Store individual fields received from the phone.
void weather_set_temperature(int temp);
void weather_set_condition(const char *cond);
void weather_set_use_celsius(bool celsius);

// Accessors used by metrics_fetch().
int         weather_get_temperature(void);
const char *weather_get_condition(void);
bool        weather_get_use_celsius(void);
