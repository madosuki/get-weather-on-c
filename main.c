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

#define TRUE 1
#define FALSE 0

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
  
  int protocol;
} url_data_s;

void get_ipaddr_from_host(struct hostent *host, char **list)
{
  if(host->h_length > 4) {
    puts("v6");
    for(int i = 0; i < host->h_length - 1; ++i) {
      printf("%d\n", (unsigned char)*host->h_addr_list[i]);
    }
  } else {
    puts("v4");
    for(int i = 0; host->h_addr_list[i]; ++i) {

      unsigned char *byte_list = INIT_ARRAY(unsigned char, 4);

      byte_list[0] = (unsigned char)*host->h_addr_list[i];
      char first[3];
      sprintf(first, "%d", byte_list[0]);
      int size = strlen(first) + 4;

      for(int j = 1; j < 4; ++j) {
        byte_list[j] = (unsigned char)*(host->h_addr_list[i] + j);
        char tmp[3];
        sprintf(tmp, "%d", byte_list[j]);
        size += strlen(tmp);
      }

      char *result = INIT_ARRAY(char, size);
      sprintf(result, "%d.%d.%d.%d", byte_list[0], byte_list[1], byte_list[2], byte_list[3]);

      free(byte_list);

      list[i] = result;

      printf("%s\n", list[i]);

    }

  }
}

int resolve_hostname(const char* hostname, char **ip_list)
{

  struct hostent *tmp;
  tmp = gethostbyname(hostname);

  if(tmp == NULL) {
    fprintf(stderr, "gethostbyname() failed: %s\n", strerror(errno));
    return -1;
  }

  get_ipaddr_from_host(tmp, ip_list);

  return 1;
}

void init_socket(socket_data_s *socket_data)
{

  int sock = socket(AF_INET, SOCK_STREAM, 0);  

  struct sockaddr_in target;
  target.sin_family = AF_INET;
  // target.sin_port = htons(HTTP_PORT);
  // target.sin_port = htons(HTTPS_PORT);

  socket_data->socket = sock;
  socket_data->target = target;
  
}

void set_addr(socket_data_s *socket_data, const url_data_s *url_data)
{
  char **ip_list = INIT_ARRAY(char*, 1024);
  int err = resolve_hostname(url_data->hostname, ip_list);

  const char* addr = "127.0.0.1";

  if(ip_list[0] != NULL)
    addr = ip_list[0];

  socket_data->target.sin_addr.s_addr = inet_addr(addr);

  for(int i = 0; i < 1024; ++i) {
    if(ip_list[i] != NULL) {
      free(ip_list[i]);
      ip_list[i] = NULL;
    }
  }

  free(ip_list);
  
}

int do_connect(socket_data_s *socket_data, int protocol, int is_ssl, const char *header)
{

  if(protocol == HTTP_PORT)
    socket_data->target.sin_port = htons(HTTP_PORT);
  else
    socket_data->target.sin_port = htons(HTTPS_PORT);

  // const char *data = "GET / HTTP/1.1\r\nHost: madosuki.github.io\r\n\r\n";

  char buf[BUF_SIZE];
  memset(buf, 0, sizeof(buf));

  long readed_size = 0;

  int err;

  connect(socket_data->socket, (struct sockaddr *)&socket_data->target, sizeof(socket_data->target));

  SSL_CTX *ctx = NULL;
  SSL *ssl = NULL;

  if(is_ssl) {
    SSL_load_error_strings();
    SSL_library_init();

    ctx = SSL_CTX_new(TLS_client_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_1_VERSION);

    ssl = SSL_new(ctx);

    err = SSL_set_fd(ssl, socket_data->socket);
    if(err == 0) {
      printf("SSL_set_fd error\n");
      SSL_free(ssl);
      SSL_CTX_free(ctx);
      close(socket_data->socket);
      return -1;
    }


    err = SSL_connect(ssl);
    if(err == 0) {
      printf("SSL_connect error\n");
      SSL_free(ssl);
      SSL_CTX_free(ctx);
      close(socket_data->socket);
      return -1;
    }

    
  }

  if(!is_ssl)
    err = write(socket_data->socket, header, strlen(header));
  else
    err = SSL_write(ssl, header, strlen(header));

  struct timeval tv;
  tv.tv_sec = 10;
  tv.tv_usec = 0;
  
  fd_set readfds, fds;
  FD_ZERO(&readfds);
  FD_SET(socket_data->socket, &readfds);
  
  int count = 0;
  /* char *result = NULL; */
  /* long result_size = 0; */
  do{

    memcpy(&fds, &readfds, sizeof(fd_set));

    err = select(socket_data->socket + 1, &fds, NULL, NULL, &tv);
    if(!err) {
      printf("timeout\n");
      break;
    }

    if(FD_ISSET(socket_data->socket, &fds)) {

      memset(buf, 0, sizeof(buf));
    
      if(!is_ssl)
        readed_size = read(socket_data->socket, buf, BUF_SIZE);
      else
        readed_size = SSL_read(ssl, buf, BUF_SIZE);

      printf("count: %d, readed size: %ld\nbody: %s\n", count, readed_size, buf);
    }

    /* result_size += readed_size; */
    /* if(result == NULL) { */
    /*   result = INIT_ARRAY(char, result_size); */
    /*   strncpy(result, buf, readed_size); */
    /* } else { */
    /*   char *check_realloc = realloc(result, result_size); */
    /*   if(check_realloc != NULL) */
    /*     strcat(result, buf); */
    /* } */

    ++count;

  } while(readed_size > 0 && count < 3);

  // printf("result data: %s\n", result);
  // free(result);

  if(is_ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    ERR_free_strings();
  }

  close(socket_data->socket);

  return 1;
}

int set_url_data(const char *url, int size, url_data_s *url_data)
{

  char check_protocol[7];
  
  for(int i = 0; i < 7; ++i) {
    check_protocol[i] = url[i];
  }

  int is_valid_protocol = -1;

  if(strcmp(check_protocol, "http://") == 0)
    is_valid_protocol = HTTP_PORT;
  else if(strcmp(check_protocol, "https:/") == 0)
    is_valid_protocol = HTTPS_PORT;
  else
    return -1;

  char *hostname = INIT_ARRAY(char, size);

  int init_value = 7;
  if(is_valid_protocol == HTTPS_PORT)
    init_value = 8;

  ssize_t pos = 0;
  for(ssize_t i = (ssize_t)init_value; i < size; ++i) {
    if(url[i] == '/')
      break;

    hostname[pos] = url[i];
    ++pos;
  }

  ssize_t path_pos = 0;
  char *path = INIT_ARRAY(char, size);
  for(ssize_t i = pos + init_value; i < size; ++i) {
    path[path_pos] = url[i];
    ++path_pos;
  }

  url_data->hostname = hostname;
  url_data->hostname_size = (ssize_t)(pos + 1);

  url_data->path_name = path;
  url_data->path_name_size = (ssize_t)(path_pos + 1);
  
  url_data->url = INIT_ARRAY(char, size);
  url_data->url_size = size;
  strncpy(url_data->url, url, size);


  return 1;
}

char* create_header(url_data_s *url_data, Method method, HttpVersion version)
{

  char *method_string = INIT_ARRAY(char, 4);
  if(method == GET)
    strncpy(method_string, "GET", 3);
  else if(method == POST)
    strncpy(method_string, "POST", 4);
  else {
    free(method_string);

    printf("unknown method!\n");

    return NULL;
  }

  char *http_version_string = INIT_ARRAY(char, 8);
  if(version == HTTP_1_1) {
    const char *http_1_1 = "HTTP/1.1";
    strncpy(http_version_string, http_1_1, strlen(http_1_1));
  } else {
    free(method_string);
    free(http_version_string);
    
    printf("invalid version!\n");

    return NULL;
  }

  const char *host_prefix = "Host: ";
  const char *header_end_line = "\r\n";

  ssize_t size = strlen(method_string) +
    url_data->path_name_size +
    strlen(http_version_string) +
    strlen(host_prefix) +
    url_data->hostname_size +
    (strlen(header_end_line) * 3) + 2;

  char *header = INIT_ARRAY(char, size);
  sprintf(header, "%s %s/ %s\r\nHost: %s\r\n\r\n", method_string, url_data->path_name, http_version_string, url_data->hostname);

  free(method_string);
  free(http_version_string);

  return header;
}

int main(void)
{
  const char *url = "https://madosuki.github.io";

  url_data_s *url_data = (url_data_s*)malloc(sizeof(url_data_s));
  int n = set_url_data(url, strlen(url), url_data);

  if(n) {
    char *header = create_header(url_data, GET, HTTP_1_1);

    if(n) {

      socket_data_s socket_data;

      init_socket(&socket_data);

      set_addr(&socket_data, url_data);

      do_connect(&socket_data, HTTPS_PORT, TRUE, header);
      //  do_connect(&socket_data, HTTP_PORT, FALSE, header);
    }

    free(header);
    header = NULL;

    free(url_data->hostname);
    free(url_data->path_name);
    free(url_data->url);

  }

  free(url_data);

  return 0;
}
