#ifndef NETWORK
#define NETWORK

#define MAX_CLIENTS 1024

typedef enum {
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_HELLO,
    STATE_MSG,
    STATE_GOODBYE,
} state_e;

typedef struct {
    int fd; // client socket file descriptor
    state_e state;
    char buffer[4096];
} client_state_t;


void init_clients(client_state_t *state);
int find_empty_poll_fds_slot(struct pollfd fds[MAX_CLIENTS+1]);
int create_and_bind_socket(short port);
int poll_and_handle_connections(int server_fd);
#endif
