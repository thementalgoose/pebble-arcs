#include <pebble.h>
#include "weather.h"

static bool s_has_data    = false;
static int  s_temperature = 0;
static bool s_use_celsius = true;
static int  s_condition   = -1;

bool weather_has_data(void) {
  return s_has_data;
}

void weather_set_temperature(int temp) {
  s_temperature = temp;
  s_has_data    = true;
}

void weather_set_condition(int cond) {
  s_condition = cond;
}

void weather_set_use_celsius(bool celsius) {
  s_use_celsius = celsius;
}

int weather_get_temperature(void) {
  return s_temperature;
}

int weather_get_condition(void) {
  return s_condition;
}

bool weather_get_use_celsius(void) {
  return s_use_celsius;
}
