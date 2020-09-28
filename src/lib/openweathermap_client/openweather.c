#include "openweather.h"

void FreeWeather(openweather_weather_s *data)
{
  if(data->descripiton != NULL)
    FREE(data->descripiton);

  if(data->icon != NULL)
    FREE(data->icon);

  if(data->main != NULL)
    FREE(data->main);

  data->id = 0;
}

void FreeOpenWeatherCurrent(openweather_map_current_s *data)
{
  if(data != NULL) {
    
    if(data->weather != NULL) {
      FreeWeather(data->weather);
      FREE(data->weather);
    }

    if(data->main != NULL)
      FREE(data->main);

    if(data->coord != NULL)
      FREE(data->coord);

    if(data->sys != NULL) {
      if(data->sys->country != NULL)
        FREE(data->sys->country);

      FREE(data->sys);
    }

    if(data->name != NULL)
      FREE(data->name);

    FREE(data);
  }
  
}


int string_equal(const char *left, const char *right)
{

  if(strcmp(left, right) == 0) {
    return TRUE;
  }

  return FALSE;
}

char *convert_json_string_to_c_string(struct json_object *obj)
{
  const char *tmp = json_object_to_json_string(obj);
  if(tmp == NULL) {
    return NULL;
  }

  ssize_t tmp_size = strlen(tmp) - 2;

  char *result = INIT_ARRAY(char, tmp_size);
  if(result == NULL) {
    return NULL;
  }

  strncpy(result, tmp + 1, tmp_size);

  return result;
}


void set_openweather_weather(struct json_object *obj, openweather_weather_s *weather)
{
  json_object_object_foreach(obj, key, val) {
    if(string_equal(key, "id")) {
      int id = json_object_get_int(val);
      weather->id = id;
    }

    if(string_equal(key, "main")) {
      weather->main = convert_json_string_to_c_string(val);
    }

    if(string_equal(key, "description")) {
      weather->descripiton = convert_json_string_to_c_string(val);
    }

    if(string_equal(key, "icon")) {
      weather->icon = convert_json_string_to_c_string(val);
    }
    
  }
}

void set_openweather_main(struct json_object *obj, openweather_main_s *main)
{
  json_object_object_foreach(obj, key, val) {
    if(string_equal(key, "temp")) {
      main->temp = json_object_get_double(val);
    }

    if(string_equal(key, "feels_like")) {
      main->feels_like = json_object_get_double(val);
    }


    if(string_equal(key, "temp_min")) {
      main->temp_min = json_object_get_double(val);
    }

    if(string_equal(key, "temp_max")) {
      main->temp_max = json_object_get_double(val);
    }

    if(string_equal(key, "pressure")) {
      main->pressure = json_object_get_int(val);
    }

    if(string_equal(key, "humidity")) {
      main->humidity = json_object_get_int(val);
    }
  }
  
}

void set_openweather_wind(struct json_object *obj, openweather_wind_s *wind)
{
  json_object_object_foreach(obj, key, val) {
    if(string_equal(key, "speed")) {
      wind->speed = json_object_get_double(val);
    }

    if(string_equal(key, "deg")) {
      wind->deg = json_object_get_double(val);
    }

    if(string_equal(key, "gust")) {
      wind->gust = json_object_get_int(val);
    }
  }
  
}

int set_openweather_clouds(struct json_object *obj)
{
  json_object_object_foreach(obj, key, val) {
    if(string_equal(key, "all")) {
      return json_object_get_int(val);
    }
  }

  return -1;
}

void set_openweather_sys(struct json_object *obj, openweather_sys_s *data)
{
  json_object_object_foreach(obj, key, val) {
    if(string_equal(key, "type")) {
      data->type = json_object_get_int(val);
    }

    if(string_equal(key, "id")) {
      data->type = json_object_get_int64(val);
    }

    if(string_equal(key, "message")) {
      data->message = json_object_get_double(val);
    }

    if(string_equal(key, "country")) {
      data->country = convert_json_string_to_c_string(val);
    }

    if(string_equal(key, "sunrise")) {
      data->sunrise = json_object_get_int64(val);
    }

    if(string_equal(key, "sunset")) {
      data->sunset = json_object_get_int64(val);
    }
    
  }
}

openweather_map_current_s *get_openweather_map_current_data(const openweather_query_s *source)
{
  const char *base = "https://api.openweathermap.org/data/2.5/weather?";
  ssize_t base_size = strlen(base);
  ssize_t url_size = base_size;

  const char *user_agent = "MyBot/1.0";

  if(source->api_key == NULL) {
    printf("require api key\n");
    return NULL;
  }

  char *appid_prefix = "&appid=";
  ssize_t appid_prefix_size = strlen(appid_prefix);
  url_size += strlen(source->api_key) + appid_prefix_size;

  if(source->city_name == NULL) {
    printf("require city name\n");
    return NULL;
  }

  char *city_name_prefix = "q=";
  ssize_t city_name_prefix_size = strlen(city_name_prefix);
  url_size += strlen(source->city_name) + city_name_prefix_size;
  
  char *url = INIT_ARRAY(char, url_size);
  strncpy(url, base, base_size);
  strncat(url, city_name_prefix, city_name_prefix_size);
  strncat(url, source->city_name, strlen(source->city_name));
  strncat(url, appid_prefix, appid_prefix_size);
  strncat(url, source->api_key, strlen(source->api_key));

  response_s *response = INIT_ARRAY(response_s, sizeof(response_s));
  int err = get_http_response(url, user_agent, response);

  FREE(url);

  if(err == -1) {
    printf("Error: failed get_http_response at main function\n");
    return NULL;
  }

  struct json_object *json_from_string = json_tokener_parse(response->body);

  if(json_from_string == NULL) {
    printf("response data is not json\n");
    FREE(response);
    
    return NULL;
  }

  openweather_map_current_s *data = (openweather_map_current_s*)malloc(sizeof(openweather_map_current_s));

  json_object_object_foreach(json_from_string, key, val) {
    if(strcmp(key, "coord") == 0) {

      openweather_coord_s *coord = (openweather_coord_s*)malloc(sizeof(openweather_coord_s));
      
      json_object_object_foreach(val, k, v) {

        if(strcmp(k, "lon") == 0) {
          coord->longitude = json_object_get_double(v);
        }

        if(strcmp(k, "lat") == 0) {
          coord->latitude = json_object_get_double(v);
        }
        
      }

      data->coord = coord;
      
    }

    if(strcmp(key, "weather") == 0) {

      openweather_weather_s *weather = (openweather_weather_s*)malloc(sizeof(openweather_weather_s));

      if(json_object_get_type(val) == json_type_array) {
        json_object *obj = json_object_array_get_idx(val, 0);
        
        set_openweather_weather(obj, weather);
        
      } else {
        
        set_openweather_weather(val, weather);
        
      }


      data->weather = weather;
    }

    if(string_equal(key, "base")) {
      data->base = convert_json_string_to_c_string(val);
    }

    if(string_equal(key, "main")) {
      openweather_main_s *main_s = (openweather_main_s*)malloc(sizeof(openweather_main_s));
      set_openweather_main(val, main_s);

      data->main = main_s;
    }

    if(string_equal(key, "visibility")) {
      int visibility = json_object_get_int(val);
      data->visibility = visibility;
    }

    if(string_equal(key, "wind")) {
      openweather_wind_s *wind = (openweather_wind_s*)malloc(sizeof(openweather_wind_s));
      set_openweather_wind(val, wind);

      data->wind = wind;
    }

    if(string_equal(key, "clouds")) {
      int all = set_openweather_clouds(val);

      if(all != -1)
        data->clouds = all;
    }

    if(string_equal(key, "dt")) {
      data->dt = json_object_get_int64(val);
    }

    if(string_equal(key, "sys")) {
      openweather_sys_s *sys = (openweather_sys_s*)malloc(sizeof(openweather_sys_s));
      set_openweather_sys(val, sys);

      data->sys = sys;
      
    }

    if(string_equal(key, "timezone")) {
      data->timezone = json_object_get_int64(val);
    }

    if(string_equal(key, "id")) {
      data->id = json_object_get_int64(val);
    }

    if(string_equal(key, "name")) {
      data->name = convert_json_string_to_c_string(val);
    }

    if(string_equal(key, "cod")) {
      data->cod = json_object_get_int(val);
    }


  }


  FREE(response);
  
  return data;
}
