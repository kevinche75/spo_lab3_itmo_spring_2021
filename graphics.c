//
// Created by kevinche on 03.06.2021.
//

#include "graphics.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include "utils.h"
#include <string.h>

int arrow_coord = 0;
int board_max = 0;
struct coordinates *start_with;
char *board_white_string;

static struct termios stored_settings;

void update(){
    printf("\033[H\033[J");
}

void goto_xy(int x, int y){
    printf("\033[%d;%dH", y, x);
}

void init_white_string(size_t size){
    board_white_string = malloc(size);
    for(int i = 0; i < size; i++) board_white_string[i] = ' ';
}

void set_keypress()
{
    struct termios new_settings;

    tcgetattr(0,&stored_settings);

    new_settings = stored_settings;

    new_settings.c_lflag &= (~ICANON & ~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(0,TCSANOW,&new_settings);
}

void reset_keypress()
{
    tcsetattr(0,TCSANOW,&stored_settings);
}

void update_arrow(int y_next){
    goto_xy(77, arrow_coord);
    puts("   ");
    goto_xy(77, y_next);
    puts("<--");
    arrow_coord = y_next;
}

void init_screen(){
    set_keypress();
    update();
    init_white_string(75);
    start_with = malloc(sizeof (struct coordinates));
    start_with->id = 0;
    start_with->column = 0;
    goto_xy(0, 0);

    for(int i = 0; i < 80; i++){
        goto_xy(i, 21);
        puts("-");
    }

    for(int i = 0; i < 21; i++){
        goto_xy(75, i);
        puts("|");
    }

    goto_xy(75, 21);
    puts("+");
    update_arrow(0);
}

void order_draw_node(struct tree *message_tree, int *visible, struct draw_status *draw_order, int level, int id){
    struct tree_node *node = &(message_tree->start)[id];
    draw_order[board_max].id = node->id;
    draw_order[board_max].level = level;
    board_max++;
    if(visible[id]){
        if(node->child > 0){
            order_draw_node(message_tree, visible, draw_order, level+1, node->child);
        }
        if(node->sibling > 0){
            order_draw_node(message_tree, visible, draw_order, level, node->sibling);
        }
    }
}

void order_draw_tree(struct tree *message_tree, int *visible, struct draw_status *draw_order){
    board_max = 0;
    order_draw_node(message_tree, visible, draw_order, 0, 0);
}

int calc_row_length(struct tree *message_tree, struct draw_status *draw_status_element){
    struct tree_node *node = &(message_tree->start)[draw_status_element->id];
    int result = draw_status_element->level*3 + 2 + 19 + 1 + strlen(node->name) + 3 + strlen(node->content);
    return result;
}

char *create_row(struct tree *message_tree, struct draw_status *draw_status_element){
    struct tree_node *node = &(message_tree->start)[draw_status_element->id];
    char *row = malloc(calc_row_length(message_tree, draw_status_element)+1);
    int i;
    for (i = 0; i < draw_status_element->level; i++){
        row[3*i] = ' ';
        row[3*i+1] = ' ';
        row[3*i+2] = ' ';
    }
    row[3*draw_status_element->level] = '\0';
    strcat(row, "+-");
    struct tm* tm_info;
    tm_info = localtime(&node->creation_time);
    i = draw_status_element->level*3+2;
    strftime(&row[i], 19, "%d.%m.%Y %H:%M:%S", tm_info);
    strcat(row, " ");
    strcat(row, node->name);
    strcat(row, " - ");
    strcat(row, node->content);
    return row;
}

void flash_board(){
    for(int i = 0; i < 20; i ++){
        goto_xy(0, i);
        puts(board_white_string);
    }
}

void draw_tree(struct tree *message_tree, struct draw_status *draw_order){
    int draw_order_i = 0;
    while (draw_order[draw_order_i].id != start_with->id) draw_order_i++;
    int board_i = 0;
    flash_board();
    char *row;
    for (; draw_order_i < board_max && board_i < 20; draw_order_i++){
        goto_xy(0, board_i);
        row = create_row(message_tree, &draw_order[draw_order_i]);
        if(strlen(&row[start_with->column]) > 75){
            row[start_with->column+75] = '\0';
        }
        puts(&row[start_with->column]);
        board_i++;
    }
}