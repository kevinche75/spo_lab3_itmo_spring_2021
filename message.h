//
// Created by kevinche on 25.05.2021.
//
#include <time.h>

#ifndef SPO_LAB3_MESSAGE_H
#define SPO_LAB3_MESSAGE_H

#endif //SPO_LAB3_MESSAGE_H

struct message{
    time_t creation_time;
    char name[20];
    char content[256];
};

struct tree_node{
    long sibling;
    long child;
    struct message content;
};