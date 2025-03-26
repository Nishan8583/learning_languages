#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef enum {
  PROTO_1,
} proto_version;

typedef enum {
  PROTO_HELLO,
} proto_type_e;

typedef struct {
  proto_type_e proto_type;
  unsigned short proto_len;

} proto_hdr_t;

void handle_server_connection(int fd) {
  char buf[4096] = {0};
  proto_hdr_t *hdr = (proto_hdr_t*)buf;

  printf("Attemtping to read\n");
  if (read(fd,hdr,sizeof(proto_hdr_t)+sizeof(int)) == -1) {
    perror("client read error");
    return;
  }

  hdr->proto_type = ntohl(hdr->proto_type);
  hdr->proto_len = htons(hdr->proto_len);

  int *data = (int*)(hdr+1);  // increase the pointer by 1 size of proto_hdr_t, we will use that as data, if no brackets, it would increase by +1
  *data = htonl(*data);
  printf("Received proto_type=%d proto_len=%d data=%d\n",hdr->proto_type,
         hdr->proto_len,*data);
  return;
}

int main(int argc, char *argv[]) {
  char *server_ip = NULL;
  if (argc < 2) {
    printf("No server IP was provided, defaulting to 127.0.0.1\n");
    server_ip = "127.0.0.1";
  } else {
    server_ip = argv[1];
  }

  struct sockaddr_in server_info = {0};
  server_info.sin_family = AF_INET;
  server_info.sin_addr.s_addr = inet_addr(server_ip);
  server_info.sin_port = htons(5555);

  int fd = socket(AF_INET, SOCK_STREAM,0);
  if (fd == -1) {
    perror("socket creation");
    return -1;
  }


    printf("Connecting to %s\n",server_ip);
  if (connect(fd, (struct sockaddr*)&server_info,sizeof(server_info)) == -1) {
    perror("connect");
    return -1;
  }

  printf("Calling server connection\n");
  handle_server_connection(fd);
  /*
  char buf[256] = {0};
  if (read(fd,&buf,sizeof(buf)) == -1) {
    perror("read");
    close(fd);
    return -1;
  }
  printf("server response %s\n",buf);
  close(fd);
  */
}
