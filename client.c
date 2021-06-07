//
// Created by kevinche on 02.06.2021.
//

#include <sys/socket.h>
#include "client.h"
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <pthread.h>
#include "graphics.h"

struct tree *client_message_tree;
int *visible;
struct draw_status *draw_order;
int server_socket;
int console_status = PRINT_VIEW;
int client_end = 0;
struct buffer message_buffer;

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
    client_message_tree = init_tree(256, 0);
    visible = init_visible_array(256);
    draw_order = init_draw_order_array(256);
    message_buffer = malloc(sizeof (struct buffer));
    struct tree_node *accepted_node = calloc(1, sizeof (struct tree_node));
    struct message *hello_message = calloc(1, sizeof (struct tree_node));
    hello_message->parent_id = -1;
    size_t count;
    if(send(server_socket, hello_message, sizeof (struct message), 0) < 0){
        perror_die("[ERROR]: Can't send Hello Message");
    }
    if(recv(server_socket, &count, sizeof (size_t), 0) > 0){
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

void *receive(void *args){

    struct tree_node *accepted_node = calloc(1, sizeof (struct tree_node));

    while (recv(server_socket, accepted_node, sizeof (struct tree_node), 0) > 0){
        insert_tree_node(client_message_tree, draw_order, visible, accepted_node);
    }
    return NULL;
}

void start_client(char* name, int port){
    creat_socket(port);
    init_receive();
    pthread_t listener;
    pthread_create(&listener, NULL, receive, NULL);
    init_screen();
    order_draw_tree(client_message_tree, visible, draw_order);
    draw_tree(client_message_tree, draw_order);
    while (!client_end){

    }
}