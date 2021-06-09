//
// Created by kevinche on 02.06.2021.
//

#ifndef SPO_LAB3_CLIENT_H
#define SPO_LAB3_CLIENT_H

#endif //SPO_LAB3_CLIENT_H

#pragma once
#include "utils.h"
#include "graphics.h"

enum{
    PRINT_VIEW = 0,
    WRITE_VIEW = 1,
};

enum{
    MAX_MESSAGE_SIZE = MESSAGE_ROW_MAX*2,
};

struct buffer{
    char content[225];
    int used;
};

void start_client(char* name, int port);