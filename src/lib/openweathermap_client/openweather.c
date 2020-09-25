#include "openweather.h"

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

  puts("json:");
  json_object_object_foreach(json_from_string, key, val) {
    if(strcmp(key, "coord") == 0) {

      openweather_coord_s coord = {};
      
      json_object_object_foreach(val, k, v) {
        // printf("%s: %s\n", k , json_object_to_json_string(v));

        if(strcmp(k, "lon") == 0) {
          const char *lon = json_object_to_json_string(v);
          char **endpoint = NULL;
          coord.logitude = strtof(lon, endpoint);
        }

        if(strcmp(k, "lat") == 0) {
          const char *lat = json_object_to_json_string(v);
          char **endpoint = NULL;
          coord.latitude = strtof(lat, endpoint);
        }
        
      }

      data->coord = coord;
      
    } else {
      printf("%s: %s\n", key, json_object_to_json_string(val));
    }
  }


  FREE(response);
  
  return data;
}
