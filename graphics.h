//
// Created by kevinche on 03.06.2021.
//

#ifndef SPO_LAB3_GRAPHICS_H
#define SPO_LAB3_GRAPHICS_H

#endif //SPO_LAB3_GRAPHICS_H
#pragma once
#include "utils.h"

struct coordinates{
    int id;
    int column;
};

void init_screen();
void order_draw_tree(struct tree *message_tree, int *visible, struct draw_status *draw_order);
void flash_board();
void draw_tree(struct tree *message_tree, struct draw_status *draw_order);

