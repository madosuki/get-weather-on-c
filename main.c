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

  url_data_s *url_data = (url_data_s*)malloc(sizeof(url_data_s));
  int err = set_url_data(url, strlen(url), GET, url_data);
  printf("hostname: %s\n", url_data->hostname);
  printf("pathname: %s\n", url_data->path_name);
  
  if(url_data->body != NULL)
    printf("body: %s\n", url_data->body);

  if(err) {
    char *header = create_header(url_data, GET, HTTP_1_1);

    if(header != NULL) {
      printf("request header: %s\n", header);

      socket_data_s socket_data;

      init_socket(&socket_data);

      set_addr(&socket_data, url_data);

      response_s response;
      do_connect(&socket_data, HTTPS_PORT, TRUE, header, &response);

      struct json_object *json_from_string = json_tokener_parse(response.body);

      json_object_object_foreach(json_from_string, key, val) {
        printf("%s: %s\n", key, json_object_to_json_string(val));
      }

      FREE(response.raw_header);
      FREE(response.body);

      FREE(header);
    }



    FREE(url_data->hostname);
    FREE(url_data->path_name);
    FREE(url_data->url);

  }

  FREE(url_data);

  return 0;
}
