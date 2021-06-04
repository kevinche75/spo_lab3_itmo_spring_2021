//
// Created by kevinche on 26.05.2021.
//

#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <unistd.h>
#include "server.h"

#define TRUE             1
#define FALSE            0
#define SERVER_PORT 44444
#define MAX_CLIENTS 20

int end_server = FALSE;

struct tree *server_message_tree;

void *launch_listener_thread (void* args)
{
    int    len, rc, on = 1;
    int    listen_sd = -1, new_sd = -1;
    int    desc_ready, compress_array = FALSE;
    int    close_conn;
    struct message *buffer = NULL;
    struct sockaddr_in addr;
    int    timeout;
    struct pollfd fds[MAX_CLIENTS+1];
    int    nfds = 1, current_size = 0, i, j;
    int updated;
    init_tree(server_message_tree, 256, 1);

    /* Create an AF_INET stream socket to receive incoming      */
    /* connections on                                            */
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0)
    {
        perror("socket() failed");
        exit(-1);
    }

    /* Allow socket descriptor to be reuseable                   */
    rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0)
    {
        perror("setsockopt() failed");
        close(listen_sd);
        exit(-1);
    }

    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    rc = ioctl(listen_sd, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(listen_sd);
        exit(-1);
    }

    /* Bind the socket                                           */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        close(listen_sd);
        perror_die("bind() failed");
    }

    /* Set the listen back log                                   */
    rc = listen(listen_sd, 32);
    if (rc < 0)
    {
        close(listen_sd);
        perror_die("listen() failed");
    }

    /* Initialize the pollfd structure                           */
    memset(fds, 0 , sizeof(fds));

    /* Set up the initial listening socket                        */
    fds[0].fd = listen_sd;
    fds[0].events = POLLIN;

    /* Initialize the timeout to 1 minute. If no                */
    /* activity after 1 minute this program will end.           */
    /* timeout value is based on milliseconds.                   */
    timeout = (60 * 1000);

    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    do
    {
        /* Call poll() and wait 1 minute for it to complete.      */
        printf("Waiting on poll()...\n");
        rc = poll(fds, nfds, timeout);

        /* Check to see if the poll call failed.                   */
        if (rc < 0)
        {
            perror("  poll() failed");
            break;
        }

        /* Check to see if the minute time out expired.          */
        if (rc == 0)
        {
            printf("  poll() timed out.  End program.\n");
            break;
        }


        /* One or more descriptors are readable.  Need to          */
        /* determine which ones they are.                          */
        current_size = nfds;
        for (i = 0; i < current_size; i++)
        {
            /* Loop through to find the descriptors that returned    */
            /* POLLIN and determine whether it's the listening       */
            /* or the active connection.                             */
            if(fds[i].revents == 0)
                continue;

            /* If revents is not POLLIN, it's an unexpected result,  */
            /* log and end the server.                               */
            if(fds[i].revents != POLLIN)
            {
                printf("  Error! revents = %d\n", fds[i].revents);
                end_server = TRUE;
                break;

            }
            if (fds[i].fd == listen_sd)
            {
                /* Listening descriptor is readable.                   */
                printf("  Listening socket is readable\n");

                /* Accept all incoming connections that are            */
                /* queued up on the listening socket before we         */
                /* loop back and call poll again.                      */
                do
                {
                    /* Accept each incoming connection. If               */
                    /* accept fails with EWOULDBLOCK, then we            */
                    /* have accepted all of them. Any other              */
                    /* failure on accept will cause us to end the        */
                    /* server.                                           */
                    new_sd = accept(listen_sd, NULL, NULL);
                    if (new_sd < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  accept() failed");
                            end_server = TRUE;
                        }
                        break;
                    }

                    /* Add the new incoming connection to the            */
                    /* pollfd structure                                  */
                    printf("  New incoming connection - %d\n", new_sd);
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;

                    //Send tree

                    send(new_sd, &(server_message_tree->used), sizeof (size_t), 0);
                    for(int node_i = 0; node_i < server_message_tree->used; node_i++){
                        send(new_sd, &(server_message_tree->start[node_i]), sizeof (struct tree_node), 0);
                    }

                    /* Loop back up and accept another incoming          */
                    /* connection                                        */
                } while (new_sd != -1);
            }
                /* This is not the listening socket, therefore an        */
                /* existing connection must be readable                  */
            else
            {
                printf("  Descriptor %d is readable\n", fds[i].fd);
                close_conn = FALSE;
                /* Receive all incoming data on this socket            */
                /* before we loop back and call poll again.            */
                do
                {
                    /* Receive data on this connection until the         */
                    /* recv fails with EWOULDBLOCK. If any other         */
                    /* failure occurs, we will close the                 */
                    /* connection.                                       */
                    rc = recv(fds[i].fd, buffer, sizeof(struct message), 0);
                    if (rc < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  recv() failed");
                            close_conn = TRUE;
                        }
                        break;
                    }
                    /* Check to see if the connection has been           */
                    /* closed by the client                              */
                    if (rc == 0)
                    {
                        printf("  Connection closed\n");
                        close_conn = TRUE;
                        break;
                    }
                    /* Data was received                                 */
                    len = rc;
                    printf("  %d bytes received\n", len);

                    updated = insert_tree_message(server_message_tree, buffer);

                    /* Echo the updated data to the clients                  */
                    for (int fd_i = 1; fd_i < nfds; fd_i++){
                        rc = send(fds[fd_i].fd, &(server_message_tree->start[updated]), sizeof (struct tree_node), 0);
                        if (rc < 0)
                        {
                            perror("  send() failed");
                            close_conn = TRUE;
                            break;
                        }

                        rc = send(fds[fd_i].fd, &(server_message_tree->start[server_message_tree->used - 1]), sizeof (struct tree_node), 0);
                        if (rc < 0)
                        {
                            perror("  send() failed");
                            close_conn = TRUE;
                            break;
                        }
                    }

                } while(TRUE);
                /* If the close_conn flag was turned on, we need       */
                /* to clean up this active connection. This            */
                /* clean up process includes removing the              */
                /* descriptor.                                         */
                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = TRUE;
                }


            }  /* End of existing connection is readable             */
        } /* End of loop through pollable descriptors              */

        /* If the compress_array flag was turned on, we need       */
        /* to squeeze together the array and decrement the number  */
        /* of file descriptors. We do not need to move back the    */
        /* events and revents fields because the events will always*/
        /* be POLLIN in this case, and revents is output.          */
        if (compress_array)
        {
            compress_array = FALSE;
            for (i = 0; i < nfds; i++)
            {
                if (fds[i].fd == -1)
                {
                    for(j = i; j < nfds; j++)
                    {
                        fds[j].fd = fds[j+1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }

    } while (end_server == FALSE); /* End of serving running.    */

    /* Clean up all of the sockets that are open                 */
    for (i = 0; i < nfds; i++)
    {
        if(fds[i].fd >= 0)
            close(fds[i].fd);
    }

    return NULL;
}

int server_mode(){
//    pthread_t listener;
//    pthread_create(&listener, NULL, launch_listener_thread, NULL);
//    while (!end_server){
//        char c = (char)getchar();
//        if(c=='Q'){
//            end_server = TRUE;
//        }
//    }

    launch_listener_thread(NULL);
    return 0;

}