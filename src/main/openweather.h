#ifndef OPENWEATHER_H
#define OPENWEATHER_H

#include <json-c/json.h>

#include "../lib/http_connector/http_connector.h"

typedef struct OpenWeatherMapCoordStruct {
  float logitude;
  float latitude;
} openweather_coord_s;

typedef struct OpenWeatherMapWeatherStruct {
  long id;
  char *main;
  char *descripiton;
  char *icon;
} openweather_weather_s;

typedef struct OpenWeatherMapMainStruct {
  float temp;
  float feels_like;
  float temp_min;
  float temp_max;
  long pressure;
  int humidity;
  int sea_level;
  int grnd_level;
} openweather_main_s;

typedef struct OpenWeatherMapWindStruct {
  float speed;
  int deg;
  int gust;
} openweather_wind_s;

typedef struct OpenWeatherMapRainStruct {
  int one_hour;
  int three_hour;
} openweather_rain_s;

typedef struct OpenWeatherMapSnowStruct {
  int one_hour;
  int three_hour;
} openweather_snow_s;

typedef struct OpenWeatherMapSysStruct {
  int type;
  int id;
  float message;
  char *country;
  long long sunrise;
  long long sunset;
} openweather_sys_s;

typedef struct OpenWeatherMapCurrent {
  openweather_coord_s coord;
  openweather_weather_s weather;
  char *base;
  openweather_main_s main;
  long visibility;
  int clouds;
  openweather_rain_s rain;
  openweather_snow_s snow;
  long long dt;
  openweather_sys_s sys;
  long long timezone;
  long long id;
  char *name;
  int cod;
} openweather_map_current_s;

typedef struct OpenWeatherMapQueryStruct {
  char *city_name;
  int city_id;
  char *country_name;
  int country_code;
  char *api_key;
  int state_code;
  char *units;
  char *mode;
  char *lang;
  int lat;
  int lon;
  int zip_code;
  char *bbox;
} openweather_query_s;

openweather_map_current_s *get_openweather_map_current_data(const openweather_query_s *source);

#endif
