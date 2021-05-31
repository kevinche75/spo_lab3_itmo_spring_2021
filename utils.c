//
// Created by kevinche on 25.05.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void perror_die(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void init_tree(struct tree *message_tree, size_t init_size){
    message_tree->start = malloc(init_size * sizeof (struct tree_node));
    message_tree->used = 0;
    message_tree->size = init_size;
    message_tree->start->id = 0;
    message_tree->start->sibling = -1;
    message_tree->start->child = -1;
    message_tree->start->parent = -1;
    strcpy(message_tree->start->content, "Board started");
    strcpy(message_tree->start->name, "root");
    message_tree->start->creation_time = time(NULL);
    message_tree->used++;
}

long insert_tree_message(struct tree* message_tree, struct message *received_message){

    //update, if needed, array size
    if (message_tree->used == message_tree->size){
        message_tree->size *= 2;
        message_tree->start = realloc(message_tree->start, message_tree->size * sizeof (struct tree_node));
    }

    //init new node
    struct tree_node *new_message = &(message_tree->start)[message_tree->used];
    strcpy(new_message->name, received_message->name);
    strcpy(new_message->content, received_message->content);
    new_message->parent = received_message->parent_id;
    new_message->child = -1;
    new_message->sibling = -1;
    new_message->id = (long)message_tree->used;
    new_message->creation_time = time(NULL);

    //find node to change
    struct tree_node *temp = &(message_tree->start)[new_message->parent];
    long updated;
    if (temp->child == -1){
        temp->child = new_message->id;
        updated = temp->id;
    } else {
        temp = &(message_tree->start)[temp->child];
        while(temp->sibling != -1){
            temp = &(message_tree->start)[temp->sibling];
        }
        temp->sibling = new_message->id;
        updated = temp->id;
    }

    //update used nodes
    message_tree->used++;
    return updated;
}