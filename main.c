#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <json-c/json.h>

#include "./lib/http_connector/http_connector.h"

int main(int argc, const char **argv)
{
  if(argc < 2) {

    printf("non argument. this program need one argument of url string\n");
    
    return -1;
  }

  const char *url = argv[1];

  const char *user_agent = "MyBot/1.0";

  response_s *response = INIT_ARRAY(response_s, sizeof(response_s));
  int err = get_http_response(url, user_agent, response);

  if(!err) {
    printf("Error: failed get_http_response at main function\n");
    return -1;
  }

  struct json_object *json_from_string = json_tokener_parse(response->body);

  if(json_from_string == NULL) {
    printf("response data is not json\n");
    FREE(response);
    
    return -1;
  }

  puts("json:");
  json_object_object_foreach(json_from_string, key, val) {
    printf("%s: %s\n", key, json_object_to_json_string(val));
  }

  FREE(response);

  return 0;
}
