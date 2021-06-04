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

int arrow_coord = 0;
struct coordinates start_with = {0, 0};
int up = 0;
int board_i = 0;

static struct termios stored_settings;

void update(){
    printf("\033[H\033[J");
}

void goto_xy(int x, int y){
    printf("\033[%d;%dH", y, x);
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
    update();
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

void draw_node(struct tree *message_tree, int *visible, int level, int id, int draw){
    struct tree_node *node = &(message_tree->start)[id];
    if (up){
        return;
    } else {
        if (start_with.id == node->id){

        }
    }
}

void draw_tree(struct tree *message_tree, int *visible){

}