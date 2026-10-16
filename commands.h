#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <stdlib.h>

#define MAX_COMMAND_LEN 5
#define N_COMMANDS 16

typedef enum {
	HLT,
	PUSH,
	PUSHR,
	POP,
	ADD,
	SUB,
	MUL,
	DIV,
	OUT,
	JMP,
	JA,
	JB,
	JAE,
	JBE,
	JE,
	JNE
} command_t;

static const char str_spu_commands[16][6] = {
	"HLT",
	"PUSH",
	"PUSHR",
	"POP",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"OUT",
	"JMP",
	"JA",
	"JB",
	"JAE",
	"JBE",
	"JE",
	"JNE"
};

static const size_t len_spu_commands[16] = {
	3, 4, 5, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 3, 2, 3
};

static const command_t list_spu_commands[16] = {
	HLT, PUSH, PUSHR, POP, ADD, SUB, MUL, DIV, OUT, JMP, JA, JB, JAE, JBE, JE, JNE
};

#endif
