//
// Created by kevinche on 03.06.2021.
//

#ifndef SPO_LAB3_GRAPHICS_H
#define SPO_LAB3_GRAPHICS_H

#endif //SPO_LAB3_GRAPHICS_H
#pragma once
#include "utils.h"

#define MESSAGE_ROW_MAX 80

struct coordinates{
    int id;
    int column;
};

void init_screen();
void reset_keypress();
void order_draw_tree(struct tree *message_tree, int *visible, struct draw_status *draw_order);
void flush_board();
void draw_tree(struct tree *message_tree, struct draw_status *draw_order);
void up(struct tree *message_tree, struct draw_status *draw_order);
void down(struct tree *message_tree, struct draw_status *draw_order);
void right(struct tree *message_tree, struct draw_status *draw_order);
void left(struct tree *message_tree, struct draw_status *draw_order);
int get_current_node();
void remove_symbol(int position);
void add_symbol(int position, char c);
void flush_message();
void redraw_tree(struct tree *client_message_tree, int* visible, struct draw_status *draw_order);
void goto_xy(int x, int y);