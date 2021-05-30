//
// Created by kevinche on 25.05.2021.
//

#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"

void perror_die(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void die(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        die("malloc failed");
    }
    return ptr;
}
