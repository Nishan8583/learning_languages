#include <asm-generic/socket.h>
#include <linux/limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#define MAX_CLIENTS 1024

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

void handle_connection(int fd) {

  // this is the way we are marshalling and unmarshalling in C
  // create a buffer
  // Define a struct, intialize a pointer to that struct, internally it points to buf
  // Now we can usae that struct struct for accessing fields
  char buf[4096] = {0}; // this will be in stack, we will use this memory for protocol; [  0, 0 , 0, 0, 0,0   ]

  proto_hdr_t *hdr = (proto_hdr_t*)buf; //
  hdr->proto_type = htonl(PROTO_HELLO);
  hdr->proto_len = htons(sizeof(int));  // len of rest of packet
  int real_len = sizeof(int);

  // increase the pointer by 1 to point to data
  // // increase the pointer by 1 size of proto_hdr_t, we will use that as data, if no brackets, it would increase by +1
  int *data = (int*)(hdr+1); // so if we treat hdr as a pointer, now we treat it as an array, first element of hdr we treat it as header data, now starting from second element we treat it as actual data
  *data = htonl(1);

  printf("Attempting to write\n");
  if (write(fd,hdr,sizeof(proto_hdr_t)+real_len) == -1) {
    perror("error server write");
    return;
  }


}

int main() {
  // 1. Create a socket;
  // 2. bind to address
  // 3. Listen
  // 4. accept
  // a socket is just a endpoint for a communication, a file descriptor
  //
  //

  // How does polling work
  // Poll takes a list of file descriptos
  // If any data is sent on any of thjese fds, we get notified
  // We loop trhough those fds, and see if POLLIN bit is set
  // if it is we read on that fd
  // max number of file descriptors that we want to poll to, plus one for server
  // Look at the G1 notes
  struct pollfd fds[MAX_CLIENTS+1];

  struct sockaddr_in server_info = {0};
  server_info.sin_family = AF_INET;
  server_info.sin_addr.s_addr = 0; //bind to all ports
  server_info.sin_port = htons(5555);

  int server_fd = socket(AF_INET,SOCK_STREAM, 0); // 1st arguement defines layer 3 protocol, SOCK_STREAM defines layer 4 protocol, final arguement idk lol
  if (server_fd == -1) {
    perror("error");
    return -1;
  }
  printf("socket successfully created fd=%d\n",server_fd);


  // Make socket reusable again
  int opt = 1;
  if (setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    perror("setsockopt");
    close(server_fd);
    return -1;
  }

  if (bind(server_fd,(struct sockaddr*)&server_info, sizeof(server_info)) == -1) {
    perror("bind error");
    close(server_fd);
    return -1;
  }

  if (listen(server_fd,0) == -1) {
    perror("listen error");
    close(server_fd);
    return -1;
  }

  struct sockaddr_in client_info = {0};
  int client_size = 0;

  socklen_t addr_len = sizeof(client_info);

  // lets set first fd as server file descriptor
  fds[0].fd = server_fd;
  fds[0].events = POLLIN; // settting events to POLLIN means we are monitoring for incomming connections
  //
  //

  for (int i = 1; i < MAX_CLIENTS +1; i++) {
      fds[i].fd = -1;
      fds[i].events=POLLIN;
      fds[i].revents = 0;
  }
  char buf[4096] = {0};

  int curr_alive_connections = 1;// poll expects the 2nd arguement to be currently active fds +

  while (1) {

    int activity = poll(fds,curr_alive_connections+1,-1); // -1 for infinite timeouts

    if (activity < 0) {
      perror("poll error");
      return -1;
    }

    // revents is sent by poll(), if the POLLIN bit is set, means there is data available to be read
    // check if server fd, which is the first one we set, has POLLIN bit set
    // which will mean we have received a new conneciton
    if (fds[0].revents & POLLIN) {
      int client_fd = accept(server_fd,(struct sockaddr*)&client_info, &addr_len);
      if (client_fd < 0) {
        perror("accept error");
        continue;
      }
    curr_alive_connections++;
      printf("New connection detected, adding to poll fds\n");
      // new conneciton is established, add it into the poll fd list
      // we loop through 1 because we do not want to replace server fd
      for (int i =1; i < MAX_CLIENTS + 1; i++) {
          if (fds[i].fd == -1) {
                fds[i].fd  = client_fd;  // we are adding it to the poll list, so that the next time the code polls, we detect client emssages
                fds[i].events = POLLIN;
                break;
          }
      }
    }

    // check clients for incomming data
    for (int i = 1; i< MAX_CLIENTS + 1; i++) {
        // if its a valid fd and if event is pollin, then we have got some data from client
        if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
            int bytes_read = read(fds[i].fd, &buf, sizeof(buf));
            if (bytes_read == 0) {
                printf("Client disconnect\n");
                close(fds[i].fd);
                fds[i].fd = -1;
            } else if (bytes_read < 0) {
                perror("client read");
                continue;
            } else { // this is the case where we got some data
                printf("Client sent us %s\n",buf);
                // just writing it back
                write(fds[i].fd,buf,bytes_read);
            }
            fds[i].revents = 0;
        }
    }


  }

  for (int i = 0; i < MAX_CLIENTS +1; i++) {
         if (fds[i].fd != -1) {
             close(fds[i].fd);
         }
     }

  /*
  char *msg = "hello friend\n";
  if (write(client_fd,msg,strlen(msg)) == -1) {  // sizeof(msg) gives size of poitner, sizeof(*msg) gives size of first value pointed, thats we use strlen
    perror("while writing");
    close(fd);
    close(client_fd);
    return -1;
  }
  */
  return 0;
}
