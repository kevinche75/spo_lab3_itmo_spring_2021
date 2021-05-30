//
// Created by kevinche on 25.05.2021.
//
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <malloc.h>

#ifndef SPO_LAB3_MESSAGE_H
#define SPO_LAB3_MESSAGE_H

// Default port
#define SERVER_PORT 44444
// Default buffer size
#define BUF_SIZE 1024
// Default timeout - http://linux.die.net/man/2/epoll_wait
#define EPOLL_RUN_TIMEOUT -1
// Count of connections that we are planning to handle (just hint to kernel)
#define EPOLL_SIZE 10000

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
void* xmalloc(size_t size);
void die(char* fmt, ...);