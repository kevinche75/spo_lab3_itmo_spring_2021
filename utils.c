//
// Created by kevinche on 25.05.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void perror_die(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}