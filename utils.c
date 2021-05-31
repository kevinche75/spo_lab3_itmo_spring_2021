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
}

long insert_tree(struct tree* message_tree, char *name, char *content, long parent_id){

    //update, if needed, array size
    if (message_tree->used == message_tree->size){
        message_tree->size *= 2;
        message_tree->start = realloc(message_tree->start, message_tree->size * sizeof (struct tree_node));
    }

    //init new node
    struct tree_node *new_message = &(message_tree->start)[message_tree->used];
    strcpy(new_message->name, name);
    strcpy(new_message->content, content);
    new_message->parent = parent_id;
    new_message->child = -1;
    new_message->sibling = -1;
    new_message->id = (long)message_tree->used;

    //find node to change
    struct tree_node *temp = &(message_tree->start)[parent_id];
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