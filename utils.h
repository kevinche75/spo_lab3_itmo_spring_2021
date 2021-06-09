//
// Created by kevinche on 25.05.2021.
//
#pragma once
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <poll.h>
#include <termios.h>
#include <signal.h>

#ifndef SPO_LAB3_MESSAGE_H
#define SPO_LAB3_MESSAGE_H

#endif //SPO_LAB3_MESSAGE_H

struct message{
    char name[20];
    char content[256];
    int parent_id;
};

struct tree_node{
    int id;
    int sibling;
    int child;
    int parent;
    time_t creation_time;
    char name[20];
    char content[256];
};

struct tree{
    struct tree_node *start;
    size_t used;
    size_t size;
};

struct updated_nodes{
    int inserted_id;
    int updated_id;
};

struct draw_status{
    int id;
    int level;
};

void perror_die(char* msg);
struct tree *init_tree(size_t init_size, int server);
int insert_tree_message(struct tree* message_tree, struct message *received_message);
void insert_tree_node(struct tree *message_tree, struct draw_status *draw_order, int *visible, struct tree_node *node);
int *init_visible_array(size_t init_size);
struct draw_status *init_draw_order_array(size_t init_size);