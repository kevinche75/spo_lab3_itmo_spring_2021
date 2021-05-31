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
    long parent_id;
};

struct tree_node{
    long id;
    long sibling;
    long child;
    long parent;
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
    long inserted_id;
    long updated_id;
};

void perror_die(char* msg);
void init_tree(struct tree *message_tree, size_t init_size);
long insert_tree_message(struct tree* message_tree, struct message *received_message);