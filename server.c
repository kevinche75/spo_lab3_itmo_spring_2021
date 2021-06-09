//
// Created by kevinche on 26.05.2021.
//

#include "server.h"

#define TRUE             1
#define FALSE            0
#define SERVER_PORT 44444
#define MAX_CLIENTS 20

struct pollfd fds[MAX_CLIENTS + 1];
int nfds = 1;

struct tree *server_message_tree;

void send_tree(int fd) {
    //Send tree
    if (send(fd, &(server_message_tree->used), sizeof(size_t), 0) < 0) {
        perror_die("[ERROR]: Can't send number of nodes in tree");
        return;
    }

    for (int node_i = 0; node_i < server_message_tree->used; node_i++) {
        if (send(fd, &(server_message_tree->start[node_i]), sizeof(struct tree_node), 0) < 0) {
            perror_die("[ERROR]: Can't send node");
            return;
        }
    }
    printf("[INFO]: Message tree was sent\n");
}

void close_connections(int dummy){
    /* Clean up all of the sockets that are open                 */

    for (int i = 0; i < nfds; i++) {
        if (fds[i].fd >= 0){
            close(fds[i].fd);
        }
    }

    printf("[INFO]: connections were closed\n");
}

void *launch_listener_thread(void *args) {
    signal(SIGINT, close_connections);
    int len, rc, on = 1;
    int listen_sd = -1, new_sd = -1;
    int compress_array = FALSE;
    struct message *buffer = calloc(1, sizeof(struct message));
    struct sockaddr_in addr;
    int current_size = 0, i, j;
    int updated;
    char *message = malloc(50);
    server_message_tree = init_tree(256, 1);

    /* Create an AF_INET stream socket to receive incoming      */
    /* connections on                                            */
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_sd < 0) {
        perror_die("[ERROR]: socket() failed");
    }

    printf("[INFO]: created socket\n");

    /* Allow socket descriptor to be reuseable                   */
    rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR,
                    (char *) &on, sizeof(on));
    if (rc < 0) {
        close(listen_sd);
        perror_die("[ERROR]: set socket reusable was failed");
    }

    printf("[INFO]: set socket reusable\n");

    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    rc = ioctl(listen_sd, FIONBIO, (char *) &on);
    if (rc < 0) {
        close(listen_sd);
        perror_die("[ERROR]: set socket non-blocking mode was failed");
    }

    /* Bind the socket                                           */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    rc = bind(listen_sd, (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        close(listen_sd);
        perror_die("[ERROR]: bind socket failed");
    }

    printf("[INFO]: socket was bounded\n");

    /* Set the listen back log                                   */
    rc = listen(listen_sd, 32);
    if (rc < 0) {
        close(listen_sd);
        perror_die("[ERROR]: set listen backlog failed");
    }

    printf("[INFO]: set listen backlog\n");

    /* Initialize the pollfd structure                           */
    memset(fds, 0, sizeof(fds));

    printf("[INFO]: initialized poll\n");

    /* Set up the initial listening socket                        */
    fds[0].fd = listen_sd;
    fds[0].events = POLLIN;

    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    printf("[INFO]: board started\n");
    do {
        /* Call poll() and wait 1 minute for it to complete.      */
        printf("[INFO]: waiting on poll\n");
        rc = poll(fds, nfds, -1);

        /* Check to see if the poll call failed.                   */
        if (rc < 0) {
            perror("[WARNING]: poll failed\n");
            break;
        }

        /* Check to see if the minute time out expired.          */
        if (rc == 0) {
            printf("[INFO]: poll timed out\n");
            break;
        }


        /* One or more descriptors are readable.  Need to          */
        /* determine which ones they are.                          */
        current_size = nfds;
        for (i = 0; i < current_size; i++) {
            /* Loop through to find the descriptors that returned    */
            /* POLLIN and determine whether it's the listening       */
            /* or the active connection.                             */
            if (fds[i].revents == 0)
                continue;

            /* If revents is not POLLIN, it's an unexpected result,  */
            /* log and end the server.                               */
            if (fds[i].revents != POLLIN) {
                sprintf(message, "[ERROR]: unexpected result, revents = %d\n", fds[i].revents);
                perror_die(message);
            }

            if (fds[i].fd == listen_sd) {
                /* Listening descriptor is readable.                   */
                printf("[INFO]: listening socket is readable\n");

                /* Accept all incoming connections that are            */
                /* queued up on the listening socket before we         */
                /* loop back and call poll again.                      */
                do {
                    /* Accept each incoming connection. If               */
                    /* accept fails with EWOULDBLOCK, then we            */
                    /* have accepted all of them. Any other              */
                    /* failure on accept will cause us to end the        */
                    /* server.                                           */
                    new_sd = accept(listen_sd, NULL, NULL);
                    if (new_sd < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror_die("[ERROR]: accept new socket failed");
                        }
                        break;
                    }

                    /* Add the new incoming connection to the            */
                    /* pollfd structure                                  */
                    printf("[INFO]: new incoming connection - %d\n", new_sd);
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;

                    /* Loop back up and accept another incoming          */
                    /* connection                                        */
                } while (new_sd != -1);
            }//                        close_conn = TRUE;

                /* This is not the listening socket, therefore an        */
                /* existing connection must be readable                  */
            else {
                printf("[INFO]: descriptor %d is readable\n", fds[i].fd);
                /* Receive all incoming data on this socket            */
                /* before we loop back and call poll again.            */
                /* Receive data on this connection until the         */
                /* recv fails with EWOULDBLOCK. If any other         */
                /* failure occurs, we will close the                 */
                /* connection.                                       */
                rc = recv(fds[i].fd, buffer, sizeof(struct message), 0);
                if (rc < 0) {
                    if (errno != EWOULDBLOCK) {
                        sprintf(message, "[ERROR]: recv failed from %i\n", fds[i].fd);
                        perror(message);
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        compress_array = TRUE;
                    }
                    break;
                }
                /* Check to see if the connection has been           */
                /* closed by the client                              */
                if (rc == 0 || rc > 0 && buffer->parent_id == -2) {
                    printf("[INFO]: connection closed, socket - %i\n", fds[i].fd);
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = TRUE;
//                    close_conn = TRUE;
                    continue;
                }
                /* Data was received                                 */
                len = rc;
                printf("[INFO]: %d bytes received from socket %i\n", len, fds[i].fd);

                if (buffer->parent_id == -1) {
                    printf("[INFO]: Message type GET_MESSAGE_TREE from socket %i\n", fds[i].fd);
                    send_tree(fds[i].fd);
                } else {
                    updated = insert_tree_message(server_message_tree, buffer);

                    /* Echo the updated data to the clients                  */
                    for (int fd_i = 1; fd_i < nfds; fd_i++) {
                        rc = send(fds[fd_i].fd, &(server_message_tree->start[updated]), sizeof(struct tree_node),
                                  0);
                        if (rc < 0) {
                            sprintf(message, "[ERROR]: send() failed to socket %i\n", fds[fd_i].fd);
                            perror(message);
                            close(fds[i].fd);
                            fds[i].fd = -1;
                            compress_array = TRUE;
                            continue;
                        }

                        rc = send(fds[fd_i].fd, &(server_message_tree->start[server_message_tree->used - 1]),
                                  sizeof(struct tree_node), 0);
                        if (rc < 0) {
                            sprintf(message, "[ERROR]: send() failed to socket %i\n", fds[fd_i].fd);
                            perror(message);
                            close(fds[i].fd);
                            fds[i].fd = -1;
                            compress_array = TRUE;
//                            close_conn = TRUE;
                            continue;
                        }
                    }

                    printf("[INFO]: updated tree was sent");
                }
            }  /* End of existing connection is readable             */
            /* If the close_conn flag was turned on, we need       */
            /* to clean up this active connection. This            */
            /* clean up process includes removing the              */
            /* descriptor.                                         */
        } /* End of loop through pollable descriptors              */

        /* If the compress_array flag was turned on, we need       */
        /* to squeeze together the array and decrement the number  */
        /* of file descriptors. We do not need to move back the    */
        /* events and revents fields because the events will always*/
        /* be POLLIN in this case, and revents is output.          */
        if (compress_array) {
            compress_array = FALSE;
            for (i = 0; i < nfds; i++) {
                if (fds[i].fd == -1) {
                    for (j = i; j < nfds; j++) {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
            printf("[INFO]: array of fds was compressed\n");
        }

    } while (TRUE); /* End of serving running.    */

    return NULL;
}

int server_mode() {
    launch_listener_thread(NULL);
    return 0;

}