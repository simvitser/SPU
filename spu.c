#include <assert.h>

#include "spu.h"
#include "common.h"
#include "commands.h"
#include "stack.h"

#define GET_AB_FROM_STACK()  long a = stackPop(&spu->stack); long b = stackPop(&spu->stack);

static void spuPUSH  (spu_t *spu);
static void spuPUSHR (spu_t *spu);
static void spuPOP   (spu_t *spu);
static void spuADD   (spu_t *spu);
static void spuSUB   (spu_t *spu);
static void spuMUL   (spu_t *spu);
static void spuDIV   (spu_t *spu);
static void spuOUT   (spu_t *spu);
static void spuJMP   (spu_t *spu);
static void spuJA    (spu_t *spu);
static void spuJB    (spu_t *spu);
static void spuJAE   (spu_t *spu);
static void spuJBE   (spu_t *spu);
static void spuJE    (spu_t *spu);
static void spuJNE   (spu_t *spu);

ErrorStatusSpu spuCtor(spu_t *spu, const char *filename) {
    assert(spu);
    assert(filename);

    char *buffer = NULL;

    ssize_t size = readFileToBuffer(filename, &buffer);

    if (size == FS_ERROR) return SPU_MEMORY_ERROR;

    spu->code = (long*)buffer;
    stackCtor(&spu->stack);
    
    return SPU_OK;
}

ErrorStatusSpu spuRun(spu_t *spu) {
    assert(spu);
    
    while (1) {
        command_t command_now = (command_t)spu->code[spu->ip];
        spu->ip++; //TODO: arg через макрос через доступ по индексу а ip++ через += 2 после выполнения
        switch (command_now) {
            case HLT:
                return SPU_OK;
                break;
	        case PUSH:  spuPUSH  (spu);  spu->ip += 2; break; // moveip(2);
	        case PUSHR: spuPUSHR (spu);  break;
            case POP:   spuPOP   (spu);  break;
            case ADD:   spuADD   (spu);  break;
            case SUB:
                spuSUB(spu);
                break;
            case MUL:
                spuMUL(spu);
                break;
            case DIV:
                spuDIV(spu);
                break;
	        case OUT:
	            spuOUT(spu);
	            break;
	        case JMP:
	            spuJMP(spu);
	            break;
	        case JA:
                spuJA(spu);
                break;
	        case JB:
	            spuJB(spu);
	            break;
	        case JAE:
	            spuJAE(spu);
	            break;
	        case JBE:
	            spuJBE(spu);
	            break;
	        case JE:
	            spuJE(spu);
	            break;
	        case JNE:
	            spuJNE(spu);
	            break;
	        default:
	            return SPU_UNKNOWN_COMMAND;
        }
    }
    return SPU_OK;
}

void spuDtor(spu_t *spu) {
    assert(spu);

    stackDtor(&spu->stack);
    free(spu->code);
}

static void spuPUSH(spu_t *spu) {
    assert(spu);

    stackPush(&spu->stack, spu->code[spu->ip++]);
}

static void spuPUSHR(spu_t *spu) {
    assert(spu);

    stackPush(&spu->stack, spu->registers[spu->code[spu->ip++]]);
}

static void spuPOP(spu_t *spu) {
    assert(spu);

    spu->registers[spu->code[spu->ip++]] = stackPop(&spu->stack);
}

static void spuADD(spu_t *spu) {    
    assert(spu);

    GET_AB_FROM_STACK();
    stackPush(&spu->stack, a + b);
}

static void spuSUB(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    stackPush(&spu->stack, b - a);
}

static void spuMUL(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    stackPush(&spu->stack, a * b);
}

static void spuDIV(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    stackPush(&spu->stack, b / a);
}

static void spuOUT(spu_t *spu) {
    assert(spu);

    long a = stackPop(&spu->stack);
	printf("%ld\n", a);
}

static void spuJMP(spu_t *spu) {
    assert(spu);

    spu->ip = spu->code[spu->ip];
}

static void spuJA(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    if (b > a) {
        spu->ip = spu->code[spu->ip];
    } else {
        spu->ip++;
    }
}

static void spuJB(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    if (b < a) {
        spu->ip = spu->code[spu->ip];
    } else {
        spu->ip++;
    }
}

static void spuJAE(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    if (b >= a) {
        spu->ip = spu->code[spu->ip];
    } else {
        spu->ip++;
    }
}

static void spuJBE(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    if (b <= a) {
        spu->ip = spu->code[spu->ip];
    } else {
        spu->ip++;
    }
}

static void spuJE(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    if (a == b) {
        spu->ip = spu->code[spu->ip];
    } else {
        spu->ip++;
    }
}

static void spuJNE(spu_t *spu) {
    assert(spu);

    GET_AB_FROM_STACK();
    if (a != b) {
        spu->ip = spu->code[spu->ip];
    } else {
        spu->ip++;
    }
}

