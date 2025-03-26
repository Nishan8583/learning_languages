#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "common.h"
#include "srvpoll.h"


void init_clients(client_state_t *state){
    for (int i = 0; i < MAX_CLIENTS; i++) {
        state[i].fd = -1;
        state[i].state = STATE_NEW;
        memset(&state[i].buffer, 0, sizeof(&state[i].buffer));
    }
}

// find_empty_slot will return the index of an empty slot
// if no emplty slot,it returns -1
int find_empty_poll_fds_slot(struct pollfd fds[MAX_CLIENTS+1]) {
    for (int i = 1; i < MAX_CLIENTS+1;i++) {
        if (fds[i].fd == -1) {
            return i;
        }
    }

    return -1;
}

int handle_client_fsm_state(client_state_t *client) {
    char *buf="hello client\n";
    if (client == NULL) {
        printf("pointer to NULL for fsm state\n");
        return STATUS_ERROR;
    }

    if (client->state == STATE_CONNECTED) {
        printf("New client at conencted state fd=%d\n",client->fd);
        client->state = STATE_HELLO;
        int bytes_written = write(client->fd, buf, strlen(buf));
        if (bytes_written == -1){
            perror("sending hello to client\n");
            client->state = STATUS_ERROR;
            return STATUS_ERROR;
        }
        printf("message has been sent %d\n",bytes_written);
        // Now we start sending emssages, so upgrade the status
        client->state = STATE_MSG;
        return STATUS_SUCCESS;
    }

    // If we are in state of receiving messages
    if (client->state == STATE_MSG) {
        int bytes_read = read(client->fd, &client->buffer, sizeof(client->buffer)); // dealing with array not buffer, so doing sizeof
        if (bytes_read == -1) {
            perror("reading");
            return STATUS_ERROR;
        } else if (bytes_read == 0) {
            printf("Client disconnect\n");
            client->fd= -1;
            client->state = STATE_NEW;
            memset(&client->buffer, '\0', sizeof(client->buffer));
            return CLIENT_DISCONNECT;
        }

        printf("Client sent us a message %s\n",client->buffer);
        memset(&client->buffer, '\0', sizeof(client->buffer));
    }
}
// 1. create a socket, bind to port, listen, and then us epoll to listen for POLLIN
// create_and_bind_socket takes in port number to listen to, creates a socket, set REUSEADDR options, binds it and listens on it
// returns server socket on success, on failure returns -1
int create_and_bind_socket(short port) {
    struct sockaddr_in server_info = {0};
    server_info.sin_family = AF_INET; // use ipv4
    server_info.sin_addr.s_addr = 0; // bind to all address
    server_info.sin_port = htons(port);


    // lets create a socket
    // if there is error after success of this call, do not forget to close this
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("server socket creation");
        return STATUS_ERROR;
    }

    // set socket options so that the address becomse reusable
    int op = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) == -1) {
        perror("setting socket options for server");
        close(server_fd);
        return STATUS_ERROR;
    }

    // now lets bind
    if (bind(server_fd, (struct sockaddr*)(&server_info), sizeof(server_info)) == -1) {
        perror("server bind error");
        close(server_fd);
        return STATUS_ERROR;
    }

    // now listen
    if (listen(server_fd, 0) == -1) {
        perror("server listen error");
        close(server_fd);
        return STATUS_ERROR;
    }

    return server_fd;
}

// Create a array of pollfds, set first to be of server, set events to POLLIN becasue we want to check on incomming data,
int poll_and_handle_connections(int server_fd) {
    if (server_fd < 0) {
        printf("poll failed, invalid file descriptor provided\n");
        return STATUS_ERROR;
    }

    struct sockaddr_in client_info = {0};

    // createing array to hold client informations and init it to default values
    client_state_t client_state_info[MAX_CLIENTS];
    init_clients(client_state_info);

    // the list of fds the poll syscall will check on
    struct pollfd fds[MAX_CLIENTS+1];

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;   // we are interesting in incomming events;

    for (int i =1 ; i < MAX_CLIENTS+1;i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }

    int active_fds = 1; //
    socklen_t addr_len = sizeof(client_info);
    while (1) {
        if (poll(fds, active_fds+1, -1) == -1) {
            perror("poll error");
            continue;
        }

        // if the server fd has revents POLLIN bit set, then a new connection has been created, lets accept it
        if (fds[0].revents & POLLIN) {
            active_fds++;
            int client_fd = accept(server_fd, (struct sockaddr*)(&client_info), &addr_len);
            if (client_fd == -1) {
                perror("server connection accept failure");
                continue;
            }

            int slot = find_empty_poll_fds_slot(fds);
            if (slot == -1) {
                printf("unable to find empty slot for new connections\n");
                continue;
            }
            fds[slot].fd = client_fd;
            fds[slot].events = POLLIN;

            client_state_info[slot].fd = client_fd;
            client_state_info[slot].state = STATE_CONNECTED;

            if (handle_client_fsm_state(&client_state_info[slot]) == STATUS_ERROR) {
                printf("error calling handline fsm state\n");
                continue;
            }

            printf("New connection accepted\n");
        }

        // Now lets see if any of the clients are sending us any data
        for (int i = 1; i < MAX_CLIENTS +1; i++) {
            // if it is a valid file descriptor
            if (fds[i].fd != -1) {
                printf("Valid file decsriptor found %d\n",fds[i].fd);
                // if client has given us some data, and OS recieved packet from NIC and POLLIN bit is set
                if (fds[i].revents & POLLIN) {
                    printf("Client file descriptor sent us some data %d\n",fds[i].fd);
                    if (handle_client_fsm_state(&client_state_info[i]) == CLIENT_DISCONNECT) {
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        fds[i].fd = POLLIN;
                        active_fds--;
                        continue;
                    }
                }
            }
        }
    }

    return STATUS_SUCCESS;

}
