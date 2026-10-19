#ifndef _SPU_H
#define _SPU_H


#include "stack.h"

typedef struct {
    long* code;
    stack_t stack;
    stack_t ret_stack;
    long registers[4];
    long ip;
} spu_t;

typedef enum {
    SPU_OK,
    SPU_MEMORY_ERROR,
    SPU_UNKNOWN_COMMAND
} ErrorStatusSpu;

ErrorStatusSpu spuCtor(spu_t *spu, const char *filename);

ErrorStatusSpu spuRun(spu_t *spu);

void spuDtor(spu_t *spu);

#endif
