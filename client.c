//
// Created by kevinche on 02.06.2021.
//

#include <sys/socket.h>
#include "client.h"
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <malloc.h>

struct tree *client_message_tree;
int *visible;
int *draw_order;
int server_socket;

int creat_socket(int port)
{

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cannot create socket\n");
        return -1;
    }

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        return -1;
    }

    if (connect(server_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    return server_socket;
}

void init_receive(){
    init_tree(client_message_tree, 256, 0);
    init_visible_array(visible, 256);
    init_draw_order(draw_order, 256);
    struct tree_node *accepted_node = calloc(1, sizeof (struct tree_node));
    int count;
    if(recv(server_socket, &count, sizeof (int), 0) > 0){
        printf("%i\n", count);
        for (int i = 0; i < count; i++){
            if(recv(server_socket, accepted_node, sizeof (struct tree_node), 0) > 0){
                insert_tree_node(client_message_tree, draw_order, visible, accepted_node);
            } else {
                perror_die("[ERROR]: Can't accept tree");
            }
        }
    } else {
        perror_die("[ERROR]: Can't accept tree");
    }
}

void receive(void *args){

    struct tree_node *accepted_node = calloc(1, sizeof (struct tree_node));

    while (recv(server_socket, accepted_node, sizeof (struct tree_node), 0) > 0){
        insert_tree_node(client_message_tree, draw_order, visible, accepted_node);
    }
}

void start_client(char* name, int port){
    int server_socket = creat_socket(port);
    init_receive();
}