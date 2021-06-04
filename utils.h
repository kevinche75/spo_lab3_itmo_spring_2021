//
// Created by kevinche on 25.05.2021.
//
#include <time.h>


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

void perror_die(char* msg);
void init_tree(struct tree *message_tree, size_t init_size, int server);
int insert_tree_message(struct tree* message_tree, struct message *received_message);
void insert_tree_node(struct tree *message_tree, int *draw_order, int *visible, struct tree_node *node);
int *init_draw_order(int *draw_order, size_t init_size);
void init_visible_array(int *visible, size_t init_size);
void init_draw_order_array(int *draw_order, size_t init_size);