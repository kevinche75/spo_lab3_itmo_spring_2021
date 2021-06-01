//
// Created by kevinche on 02.06.2021.
//

#include <sys/socket.h>
#include "client.h"
#include <netinet/in.h>
#include <stdio.h>

int creat_socket(int port)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (bindResult == -1)
    {
        perror_die("bindResult");
    }

    int listenResult = listen(server_socket, 5);
    if (listenResult == -1)
    {
        perror_die("listenResult");
    }
    printf("server start\n");
    return server_socket;
}

void start_client(char* name, int port){
    int server_socket = creat_socket(port);
}