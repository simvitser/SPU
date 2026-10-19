#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <stdlib.h>
#include <stdint.h>

#define MAX_COMMAND_LEN 8
#define N_COMMANDS 18

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
	JNE,
	CALL,
	RET,
	UNKNOWN_COMMAND
} command_t;

static const char str_spu_commands[N_COMMANDS][MAX_COMMAND_LEN + 1] = {
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
	"JNE",
	"CALL",
	"RET"
};

static const uint64_t hash_spu_commands[N_COMMANDS] = {
	5524552,
	1213420880,
	353400739152,
	5263184,
	4473921,
	4347219,
	5002573,
	5654852,
	5526863,
	5262666,
	16714,
	16970,
	4538698,
	4538954,
	17738,
	4542026,
	1280065859,
	5522770
};

static const size_t len_spu_commands[N_COMMANDS] = {
	3, 4, 5, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 3, 2, 3, 4, 3
};

static const command_t list_spu_commands[N_COMMANDS] = {
	HLT, PUSH, PUSHR, POP, ADD, SUB, MUL, DIV, OUT, JMP, JA, JB, JAE, JBE, JE, JNE, CALL, RET
};

#endif
