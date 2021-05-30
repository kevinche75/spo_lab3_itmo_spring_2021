//
// Created by kevinche on 25.05.2021.
//
#include <sys/time.h>


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

void perror_die(char* msg);