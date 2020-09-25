#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <json-c/json.h>

#include "../lib/openweathermap_client/openweather.h"


int main(int argc, const char **argv)
{
  if(argc < 2) {

    printf("non argument. this program need one argument of url string\n");
    
    return -1;
  }

  struct stat stat_data;

  ssize_t home_directory_path_size = strlen(getenv("HOME"));
  const char *setting_file_dir_path_parts = "/.config/openweather_map_client/";
  ssize_t setting_file_dir_path_parts_size = strlen(setting_file_dir_path_parts);
  
  const char *setting_file_name = "openweather_map_account.json";
  ssize_t setting_file_name_size = strlen(setting_file_name);
  
  char *setting_file_dir_path = INIT_ARRAY(char, home_directory_path_size + setting_file_dir_path_parts_size);
  strncpy(setting_file_dir_path, getenv("HOME"), home_directory_path_size);
  strncat(setting_file_dir_path, setting_file_dir_path_parts, setting_file_dir_path_parts_size);

  printf("%s\n", setting_file_dir_path);
 
  if(stat(setting_file_dir_path, &stat_data) == -1) {
    mkdir(setting_file_dir_path, 0755);
  }

  char *setting_file_path = INIT_ARRAY(char, home_directory_path_size +
                                      setting_file_dir_path_parts_size +
                                      setting_file_name_size);

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
  query.api_key = INIT_ARRAY(char, api_key_size);
  strncpy(query.api_key, api_key + 1, api_key_size - 2);

  ssize_t city_name_size = strlen(argv[1]);
  query.city_name = INIT_ARRAY(char, city_name_size);
  strncpy(query.city_name, argv[1], city_name_size);

  printf("api_key: %s\ncity name: %s\n", query.api_key, query.city_name);

  openweather_map_current_s *result = get_openweather_map_current_data(&query);
  FREE(query.api_key);
  FREE(query.city_name);
  FREE(result);

  return 0;
}
