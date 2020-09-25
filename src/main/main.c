#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "../lib/openweathermap_client/openweather.h"


int main(int argc, const char **argv)
{
  if(argc < 2) {

    printf("non argument. this program need one argument of url string\n");
    
    return -1;
  }

  openweather_query_s query = {};
  query.api_key = INIT_ARRAY(char, strlen(argv[1]));
  strncpy(query.api_key, argv[1], strlen(argv[1]));

  query.city_name = "London";

  openweather_map_current_s *result = get_openweather_map_current_data(&query);
  FREE(result);

  return 0;
}
