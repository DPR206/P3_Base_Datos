#include <stdio.h>
#include <stdlib.h>

#include "command.h"

#define OK 0
#define ERR 1

#define INIT_SIZE 2

typedef enum {
    UNKNOWN = -1,
    ADD,
    FIND,
    DEL,
    EXIT,
    PRINTIND, 
    PRINTLST,
    PRINTREC
} Command;