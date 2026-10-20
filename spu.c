#include <assert.h>
#include <string.h>

#include "spu.h"
#include "common.h"
#include "commands.h"
#include "stack.h"

#define GET_AB_FROM_STACK()  long a = stackPop(&spu->stack); long b = stackPop(&spu->stack);

static void spuPUSH  (spu_t *spu);
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
static void spuCALL  (spu_t *spu);
static void spuRET   (spu_t *spu);
static void spuVOUT  (spu_t *spu);
static void spuIN    (spu_t *spu);

static long spuGetArg(spu_t *spu);

#define spuJMPC(name, op) \
static void spu##name(spu_t *spu) { \
    assert(spu); \
\
    GET_AB_FROM_STACK(); \
    if (b op a) { \
        spu->ip = spuGetArg(spu); \
    } else { \
        spu->ip += (long)sizeof(long); \
    } \
}

#define CASE_RUN(name) case name: spu##name(spu); break;


ErrorStatusSpu spuCtor(spu_t *spu, const char *filename) {
    assert(spu);
    assert(filename);

    char *buffer = NULL;

    ssize_t size = readFileToBuffer(filename, &buffer);

    if (size == FS_ERROR) return SPU_MEMORY_ERROR;

    spu->code = (uint8_t*)buffer;

    stackCtor(&spu->ret_stack);
    stackCtor(&spu->stack);
    
    return SPU_OK;
}

ErrorStatusSpu spuRun(spu_t *spu) {
    assert(spu);
    
    while (1) { 
        command_t command_now = (command_t)spu->code[spu->ip];
        spu->ip++;
         
        switch (command_now & OPCODE_MASK) {
            case HLT:
                return SPU_OK;
                break;
            CASE_RUN(PUSH);
            case POP:   spuPOP   (spu); break;
            case ADD:   spuADD   (spu); break;
            case SUB:   spuSUB   (spu); break;
            case MUL:   spuMUL   (spu); break;
            case DIV:   spuDIV   (spu); break;
            case OUT:   spuOUT   (spu); break;
            case JMP:   spuJMP   (spu); break;
            case JA:    spuJA    (spu); break;
            case JB:    spuJB    (spu); break;
            case JAE:   spuJAE   (spu); break;
            case JBE:   spuJBE   (spu); break;
            case JE:    spuJE    (spu); break;
            case JNE:   spuJNE   (spu); break;
            case CALL:  spuCALL  (spu); break;
            case RET:   spuRET   (spu); break;
            CASE_RUN(VOUT);
            CASE_RUN(IN);
            default:
                return SPU_UNKNOWN_COMMAND;
        }
        
        // printf("command_n: %d\n", command_now & OPCODE_MASK);
        // printf("command_now: %s\n", str_spu_commands[command_now & OPCODE_MASK]);
        // stackPrint(&spu->stack);
    }
    return SPU_OK;
}

void spuDtor(spu_t *spu) {
    assert(spu);

    stackDtor(&spu->stack);
    stackDtor(&spu->ret_stack);
    free(spu->code);
}

static void spuPUSH(spu_t *spu) {
    assert(spu);

    long el = spuGetArg(spu);

    if (spu->code[spu->ip - 1] & REG_MASK) {
        el = spu->registers[el];
    }

    if (spu->code[spu->ip - 1] & RAM_MASK) {
        el = spu->mem[el];
    }

    stackPush(&spu->stack, el);

    spu->ip += (long)sizeof(long);
}

static void spuPOP(spu_t *spu) {
    assert(spu);

    long el = spuGetArg(spu);
 
    if ((spu->code[spu->ip - 1] & REG_MASK) && (spu->code[spu->ip - 1] & RAM_MASK)) {
        spu->mem[spu->registers[el]] = stackPop(&spu->stack);
    } else if (spu->code[spu->ip - 1] & REG_MASK) {
        spu->registers[el] = stackPop(&spu->stack);
    } else if (spu->code[spu->ip - 1] & RAM_MASK) {
        spu->mem[el] = stackPop(&spu->stack);
    } else {
        printfError("watafa pop to const?!\n");
        abort();
    }

    spu->ip += (long)sizeof(long);
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

    spu->ip = spuGetArg(spu);
}

spuJMPC(JA,  > );
spuJMPC(JB,  < );
spuJMPC(JAE, >=);
spuJMPC(JBE, <=);
spuJMPC(JE,  ==);
spuJMPC(JNE, !=);

static void spuCALL(spu_t *spu) {
    assert(spu);

    stackPush(&spu->ret_stack, spu->ip + sizeof(long));
    spu->ip = spuGetArg(spu);
}

static void spuRET(spu_t *spu) {
    spu->ip = stackPop(&spu->ret_stack);
}

static long spuGetArg(spu_t *spu) {
    long ans = 0;
    memcpy(&ans, &spu->code[spu->ip], sizeof(long));
    return ans;
}

static void spuVOUT(spu_t *spu) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf(spu->mem[i * 10 + j] ? "*" : ".");
        }
        printf("\n");
    }
}

static void spuIN(spu_t *spu) {
    printf(">>> ");
    long a = 0;
    scanf("%ld", &a);
    stackPush(&spu->stack, a);
}
