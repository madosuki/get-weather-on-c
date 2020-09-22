#ifndef HTTP_CONNECTOR_H
#define HTTP_CONNECTOR_H

#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <openssl/err.h>
#include <openssl/ssl3.h>

#define HTTP_PORT 80
#define HTTPS_PORT 443

#define BUF_SIZE 4096

#define MAX_HTTP_HEADER_SIZE 16384

#define FREE(v) \
  do {          \
    free(v);\
    v = NULL;\
  } while(0);

#define INIT_ARRAY(type, size) (type*)calloc(size, sizeof(type))

typedef enum {
  GET,
  POST
} Method;

typedef enum {
  HTTP_1_1
} HttpVersion;

typedef struct SocketDataStruct {
  int socket;
  struct sockaddr_in target;
} socket_data_s;

typedef struct UrlDataStruct {
  char *url;
  ssize_t url_size;
  
  char *hostname;
  ssize_t hostname_size;
  
  char *path_name;
  ssize_t path_name_size;

  char *body;
  ssize_t body_size;
  
  int protocol;
} url_data_s;

typedef struct ResponseStruct
{
  char *raw_header;
  char **header_list;
  char *body;
  ssize_t raw_header_size;
  ssize_t body_size;
  ssize_t header_list_size;
} response_s;

int set_http_response_data(const char *response_data, ssize_t size, response_s *result);

void get_ipaddr_from_host(struct hostent *host, char **list);

int resolve_hostname(const char* hostname, char **ip_list);

void init_socket(socket_data_s *socket_data);

void set_addr(socket_data_s *socket_data, const url_data_s *url_data);

int do_connect(socket_data_s *socket_data, int protocol, int is_ssl, const char *data, response_s *response);

int set_url_data(const char *url, int size, Method method, url_data_s *url_data);

char* create_header(url_data_s *url_data, Method method, HttpVersion version);

#endif
