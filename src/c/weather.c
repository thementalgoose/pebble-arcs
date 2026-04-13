#include <pebble.h>
#include "weather.h"

static bool s_has_data    = false;
static int  s_temperature = 0;
static bool s_use_celsius = true;
static char s_condition[8] = "--";

bool weather_has_data(void) {
  return s_has_data;
}

void weather_set_temperature(int temp) {
  s_temperature = temp;
  s_has_data    = true;
}

void weather_set_condition(const char *cond) {
  strncpy(s_condition, cond, sizeof(s_condition) - 1);
  s_condition[sizeof(s_condition) - 1] = '\0';
}

void weather_set_use_celsius(bool celsius) {
  s_use_celsius = celsius;
}

int weather_get_temperature(void) {
  return s_temperature;
}

const char *weather_get_condition(void) {
  return s_condition;
}

bool weather_get_use_celsius(void) {
  return s_use_celsius;
}
