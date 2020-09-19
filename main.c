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

#define TRUE 1
#define FALSE 0

#define INIT_ARRAY(type, size) (type*)calloc(size, sizeof(type))

typedef struct SocketDataStruct {
  int socket;
  struct sockaddr_in target;
} socket_data_s;

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

void set_addr(socket_data_s *socket_data, const char *hostname)
{
  char **ip_list = INIT_ARRAY(char*, 1024);
  int err = resolve_hostname(hostname, ip_list);

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

int do_connect(socket_data_s *socket_data, int protocol, int is_ssl)
{

  if(protocol == HTTP_PORT)
    socket_data->target.sin_port = htons(HTTP_PORT);
  else
    socket_data->target.sin_port = htons(HTTPS_PORT);

  const char *location = "/";
  const char *data = "GET / HTTP/1.1\r\nHost: madosuki.github.io\r\n\r\n";

  char buf[4096];
  memset(buf, 0, sizeof(buf));

  long readed_size = 0;

  connect(socket_data->socket, (struct sockaddr *)&socket_data->target, sizeof(socket_data->target));

  if(is_ssl) {
    SSL_load_error_strings();
    SSL_library_init();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_1_VERSION);

    // SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());

    SSL *ssl = SSL_new(ctx);

    int err = SSL_set_fd(ssl, socket_data->socket);
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

    err = SSL_write(ssl, data, strlen(data));

    do {
      // readed_size = read(sock, buf, sizeof(buf));
      readed_size = SSL_read(ssl, buf, sizeof(buf));

      printf("ssl readed size: %ld\nbody: %s\n", readed_size, buf);

    } while(readed_size > 0);


    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    ERR_free_strings();


    
  } else {
      int err = write(socket_data->socket, data, strlen(data));


      do {

        readed_size = read(socket_data->socket, buf, sizeof(buf));

        printf("non ssl readed size: %ld\nbody: %s\n", readed_size, buf);

      } while(readed_size > 0);

  }

  close(socket_data->socket);

  return 1;
}

int main(void)
{

  socket_data_s socket_data;

  init_socket(&socket_data);

  set_addr(&socket_data, "madosuki.github.io");

  do_connect(&socket_data, HTTPS_PORT, TRUE);

  return 0;
}
