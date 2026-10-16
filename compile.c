#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "common.h"
#include "commands.h"

#define MAX_CODE_LEN 10000
#define MAX_ARG_LEN  100
#define MAX_TAGS     1000

command_t parse_command(const char *command_now);

long get_register_number(const char* arg);

int get_arg(char *arg, const char *buffer);

void delete_comments(char *comment_ptr);

int main(const int argc, const char *argv[]) {
    if (argc <= 1) {
        printfError("no file\n");
        return 1;
    }

    char *real_buffer = NULL;
    ssize_t buffer_len = readFileToBuffer(argv[1], &real_buffer);
    char *buffer = real_buffer;

    if (buffer_len == FS_ERROR) {
        printfError("file doesn't exist\n");
        return 1;
    }
    
    delete_comments(buffer); 

    FILE *file = fopen("asm.bin", "wb");
    if (file == NULL) {
        printfError("can't write file\n");
        return 1;
    }

    printfOK("start compile\n");
    long code[MAX_CODE_LEN] = {};
    long *code_ptr = code;
    
    char command_str[MAX_COMMAND_LEN + 1] = {};
    int command_len = 0;
    
    long tags[MAX_TAGS] = {};
    for (size_t i = 0; i < MAX_TAGS; i++) {
        tags[i] = -1;
    }
    
    long tags2fix[MAX_CODE_LEN * 2] = {};
    for (size_t i = 0; i < MAX_CODE_LEN * 2; i++) {
        tags2fix[i] = -1;
    }
    
    while (sscanf(buffer, "%s%n", command_str, &command_len) == 1) {
        printf("command_now: %s len: %d\n", command_str, command_len);

        buffer += command_len;

        if (*command_str == ':') {
            long tag = strtol(command_str + 1, NULL, 10);            
            if (tags[tag] != -1) {
                printfError("duplicate tag\n");
                return -1;
            }
            tags[tag] = code_ptr - code;
            continue;
        } 

        command_t command = parse_command(command_str);

        char arg[MAX_ARG_LEN] = {};
        int shift = 0;
        long arg_l = 0;

        switch (command) {
            case HLT:
                *code_ptr = (long)command;
                code_ptr++;
                break;
	        case PUSH:
	            shift = get_arg(arg, buffer);
	            if (shift == -1) {
	                printfError("no arg push: %s\n", arg);
	                return 1;
	            }
	            buffer += shift;

	            if (*buffer != '\n') {
	                printfError("too much args push\n");
	                return 1;
	            }

	            if (isalpha(*arg)) {
	                *code_ptr = (long)PUSHR;
	                code_ptr++;
	                arg_l = get_register_number(arg);
	                if (arg_l == -1) {
	                    printfError("unknown register %s\n", arg);
	                    return 1;
	                }
	                *code_ptr = arg_l;
	                code_ptr++;
	            } else {
	                *code_ptr = (long)PUSH;
	                code_ptr++;
	                arg_l = strtol(arg, NULL, 10);
	                *code_ptr = arg_l;
	                code_ptr++;
	            }
	            break;
	        case PUSHR:
	        case POP:
	            shift = get_arg(arg, buffer);
	            if (shift == -1) {
	                printfError("no arg pushr/pop: %s\n", arg);
	                return 1;
	            }
	            buffer += shift;

	            if (*buffer != '\n') {
	                printfError("too much args pushr/pop\n");
	                return 1;
	            }

	            if (isalpha(*arg)) {
	                *code_ptr = (long)command;
	                code_ptr++;
	                arg_l = get_register_number(arg);
	                if (arg_l == -1) {
	                    printfError("unknown register %s\n", arg);
	                    return 1;
	                }
	                *code_ptr = arg_l;
	                code_ptr++;
	            } else {
	                printfError("register should start with letter, got: %s\n", arg);
	                return 1;
	            }
	            break;
	        case ADD:
	        case SUB:
	        case MUL:
	        case DIV:
	        case OUT:
	            if (*buffer != '\n') {
	                printfError("arg at add/mul/div/out\n");
	                return 1;
	            }
	            buffer++;
	            *code_ptr = (long)command;
	            code_ptr++;
	            break;
	        case JMP:
	        case JA:
	        case JB:
	        case JAE:
	        case JBE:
	        case JE:
            case JNE:
	            shift = get_arg(arg, buffer);
	            if (shift == -1) {
	                printfError("no arg JMP: %s\n", arg);
	                return 1;
	            }
	            buffer += shift;

	            if (*buffer != '\n') {
	                printfError("too much args JMP\n");
	                return 1;
	            }

	            arg_l = strtol(arg, NULL, 10);
	            if (arg_l == -1) {
	                printfError("unknown register %s\n", arg);
	                return 1;
	            }

	            *code_ptr = (long)command;
	            code_ptr++;

	            if (tags[arg_l] != -1) {
	                *code_ptr = tags[arg_l];
	            } else {
	                *code_ptr = 0;
	                tags2fix[code_ptr - code] = arg_l;
	            }
	            code_ptr++;
	           break;
	        default:
	            printfError("unknown command\n");
	            return 1;
        }
        
        for (size_t i = 0; i < MAX_COMMAND_LEN + 1; i++) command_str[i] = 0;
    }
    printfOK("end first compile\n");

    for (size_t i = 0; i < code_ptr - code; i++) {
        if (tags2fix[i] != -1) {
            code[i] = tags[tags2fix[i]];
        }
    }

    for (size_t i = 0; i < (code_ptr - code) * 8; i++) {
        fprintf(file, "%c", ((char*)code)[i]);
    }

    for (size_t i = 0; i < code_ptr - code; i++) {
        printf("%ld ", code[i]);
    }
    printf("\n");

    printfOK("end second compile\n");


    free(real_buffer);
    fclose(file);
    return 0;
}

command_t parse_command(const char *command_now) {
    assert(command_now);

    for (int i = 0; i < N_COMMANDS; i++) {
        if (!strncmp(command_now, str_spu_commands[i], len_spu_commands[i])) {
            return list_spu_commands[i];
        }
    }
    printfError("unknown command: %10s\n", command_now);
    return HLT;
}

long get_register_number(const char* arg) {
    assert(arg);

    if (!strncmp(arg, "AX", 2)) return 0;
    if (!strncmp(arg, "BX", 2)) return 1;
    if (!strncmp(arg, "CX", 2)) return 2;
    if (!strncmp(arg, "DX", 2)) return 3;
    return -1;
}

int get_arg(char *arg, const char *buffer) {
    assert(arg);
    assert(buffer);

    memset(arg, '\0', MAX_ARG_LEN);
    int arglen = 0;
	if (sscanf(buffer, "%s%n", arg, &arglen) != 1) return -1;
	while (buffer[arglen] == ' ') arglen++;
	return arglen;
}

void delete_comments(char *comment_ptr) {
    while (*comment_ptr != '\0') {
        while (*comment_ptr != ';') comment_ptr++;
        while (*comment_ptr != '\n' && *comment_ptr != '\0') {
            *comment_ptr = ' ';
            comment_ptr++;
        }
    }
}
