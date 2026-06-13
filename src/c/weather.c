#include <pebble.h>
#include "weather.h"

static const uint32_t PERSIST_KEY_CACHED_WEATHER_TEMPERATURE = 0xFEF1;
static const uint32_t PERSIST_KEY_CACHED_WEATHER_CONDITION   = 0xFEF2;

static bool s_has_temperature_data = false;
static bool s_has_condition_data   = false;
static int  s_temperature = 0;
static bool s_use_celsius = true;
static int  s_condition   = -1;

bool weather_has_data(void) {
  return s_has_temperature_data || s_has_condition_data;
}

void weather_set_temperature(int temp) {
  s_temperature = temp;
  s_has_temperature_data = true;
  persist_write_int(PERSIST_KEY_CACHED_WEATHER_TEMPERATURE, temp);
}

void weather_set_condition(int cond) {
  s_condition = cond;
  s_has_condition_data = true;
  persist_write_int(PERSIST_KEY_CACHED_WEATHER_CONDITION, cond);
}

bool weather_has_temperature_data(void) {
  return s_has_temperature_data;
}

bool weather_has_condition_data(void) {
  return s_has_condition_data;
}

void weather_set_use_celsius(bool celsius) {
  s_use_celsius = celsius;
}

void weather_restore_cached_data(void) {
  if (persist_exists(PERSIST_KEY_CACHED_WEATHER_TEMPERATURE)) {
    s_temperature = persist_read_int(PERSIST_KEY_CACHED_WEATHER_TEMPERATURE);
    s_has_temperature_data = true;
  }
  if (persist_exists(PERSIST_KEY_CACHED_WEATHER_CONDITION)) {
    s_condition = persist_read_int(PERSIST_KEY_CACHED_WEATHER_CONDITION);
    s_has_condition_data = true;
  }
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
