#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <json-c/json.h>

#include "../lib/openweathermap_client/openweather.h"


int main(int argc, const char **argv)
{
  if(argc < 2) {

    printf("non argument. this program need one argument of url string\n");
    
    return -1;
  }

  struct json_object *account = json_object_from_file("./openweather_map_account.json");
  if(account == NULL) {
    printf("Error: openweather_map_account.json is not found\n");
    return -1;
  }
  
  struct json_object *tmp = json_object_object_get(account, "api_key");
  if(tmp == NULL) {
    printf("Error: missing api_key in openweather_map_account.json\n");
    return -1;
  }
  const char *api_key = json_object_to_json_string(tmp);
  ssize_t api_key_size = strlen(api_key);

  openweather_query_s query = {};
  query.api_key = INIT_ARRAY(char, api_key_size);
  strncpy(query.api_key, api_key + 1, api_key_size - 2);

  ssize_t city_name_size = strlen(argv[1]);
  query.city_name = INIT_ARRAY(char, city_name_size);
  strncpy(query.city_name, argv[1], city_name_size);

  openweather_map_current_s *result = get_openweather_map_current_data(&query);
  FREE(query.api_key);
  FREE(query.city_name);
  FREE(result);

  return 0;
}
