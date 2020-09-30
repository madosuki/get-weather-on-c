#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#endif
#include <sys/stat.h>

#include <json-c/json.h>

#include "../lib/openweathermap_client/openweather.h"


int main(int argc, const char **argv)
{
  if(argc < 2) {

    printf("non argument. This program need one argument which named City Name in OpenWeatherMap API.\n");
    
    return -1;
  }

  #ifdef _WIN32
  struct _stat stat_data;
  #else
  struct stat stat_data;
  #endif


  #ifdef _WIN32
  char *home_directory = getenv("USER");
  #else
  char *home_directory = getenv("HOME");
  #endif
  
  ssize_t home_directory_path_size = strlen(home_directory);
  #ifdef _WIN32
  const char *setting_file_dir_path_parts = "\\.openweather_map_client\\";
  #else
  const char *setting_file_dir_path_parts = "/.config/openweather_map_client/";
  #endif
  
  ssize_t setting_file_dir_path_parts_size = strlen(setting_file_dir_path_parts);
  
  const char *setting_file_name = "openweather_map_account.json";
  ssize_t setting_file_name_size = strlen(setting_file_name);

  ssize_t setting_file_dir_path_size = home_directory_path_size + setting_file_dir_path_parts_size;
  char *setting_file_dir_path = INIT_ARRAY(char, setting_file_dir_path_size + 1);
  strncpy(setting_file_dir_path, home_directory, home_directory_path_size);
  strncat(setting_file_dir_path, setting_file_dir_path_parts, setting_file_dir_path_parts_size);

  printf("%s\n", setting_file_dir_path);

  #ifdef _WIN32
  if(_stat(setting_file_dir_path, &stat_data) != 0) {
    _mkdir(setting_file_dir_path);
  }
  #else
  if(stat(setting_file_dir_path, &stat_data) == -1) {
    mkdir(setting_file_dir_path, 0755);
  }
  #endif

  ssize_t setting_file_path_size = home_directory_path_size +
    setting_file_dir_path_parts_size +
    setting_file_name_size;
  char *setting_file_path = INIT_ARRAY(char, setting_file_path_size + 1);

  strncpy(setting_file_path,
          setting_file_dir_path, home_directory_path_size + setting_file_dir_path_parts_size);

  strncat(setting_file_path, setting_file_name, setting_file_name_size);

  if(stat(setting_file_dir_path, &stat_data) == -1) {
    printf("Error: openweather_map_account.json is not found\n");
    
    FREE(setting_file_path);
    FREE(setting_file_dir_path);
    
    return -1;
  }
  

  struct json_object *account = json_object_from_file(setting_file_path);
  if(account == NULL) {
    printf("Error: load error %s\n", setting_file_path);
    
    FREE(setting_file_path);
    FREE(setting_file_dir_path);
    
    return -1;
  }
  FREE(setting_file_path);
  FREE(setting_file_dir_path);
  
  struct json_object *tmp = json_object_object_get(account, "api_key");
  if(tmp == NULL) {
    printf("Error: missing api_key in openweather_map_account.json\n");
    
    return -1;
  }
  const char *api_key = json_object_to_json_string(tmp);
  ssize_t api_key_size = strlen(api_key);

  openweather_query_s query = {};
  query.api_key = INIT_ARRAY(char, api_key_size + 1);
  strncpy(query.api_key, api_key + 1, api_key_size - 2);
  query.api_key[api_key_size] = '\0';

  ssize_t city_name_size = strlen(argv[1]);
  query.city_name = INIT_ARRAY(char, city_name_size + 1);
  strncpy(query.city_name, argv[1], city_name_size);
  query.city_name[city_name_size] = '\0';

  printf("api_key: %s\ncity name: %s\n", query.api_key, query.city_name);

  openweather_map_current_s *result = get_openweather_map_current_data(&query);
  if(result->weather->main != NULL) {
    printf("coord:\nlon: %f, lat: %f\n\nweather:\nid: %ld\nmain: %s\n", result->coord->longitude, result->coord->latitude, result->weather->id, result->weather->main);
    printf("sys:\ncountry: %s\n", result->sys->country);

  }

  FreeOpenWeatherCurrent(result);
  
  FREE(query.api_key);
  FREE(query.city_name);

  return 0;
}
