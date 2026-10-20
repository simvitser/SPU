#define DEBUG if (1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "common.h"
#include "commands.h"

#define MAX_CODE_LEN 10000
#define MAX_ARG_LEN  100
#define MAX_TAG_LEN  100
#define MAX_TAGS     1000

#define GET_ARG(CMD) \
shift = get_arg(arg, buffer); \
if (shift == -1) { \
    printfError("no arg " CMD ": %s\n", arg); \
    return 1; \
} \
buffer += shift; \
NO_ARG(CMD);

#define NO_ARG(CMD) \
while (*buffer == ' ') buffer++; \
if (*buffer != '\n') { \
    printfError("too much args at " CMD "\n"); \
    return 1; \
}

#define PUT_ARG(code_ptr, arg) \
memcpy(code_ptr, &arg, sizeof(arg)); \
code_ptr += sizeof(arg);

command_t parse_command(const char *command_now);

long get_register_number(const char* arg);

int get_arg(char *arg, const char *buffer);

void delete_comments(char *comment_ptr);

int get_tag(const uint64_t tag, const long tags_now, uint64_t *hash_tags);

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
    uint8_t code[MAX_CODE_LEN] = {};
    uint8_t *code_ptr = code;
    
    char command_str[MAX_TAG_LEN + 1] = {};
    int command_len = 0;
    
    long tags[MAX_TAGS] = {};
    for (size_t i = 0; i < MAX_TAGS; i++) {
        tags[i] = -1;
    }

    uint64_t hash_tags[MAX_TAGS] = {}; 

    long tags_now = 0;
    uint64_t hash_tag_now = 0;
    while (sscanf(buffer, "%s%n", command_str, &command_len) == 1) {
        DEBUG printf("command_now: %-8s hash: %-24lu len: %d\n", command_str, getHash8char(command_str), command_len);

        buffer += command_len;

        if (*command_str == ':') {
            hash_tags[tags_now] = getHashString(command_str + 1);
            int tag = get_tag(hash_tags[tags_now], tags_now, hash_tags);
            if (tag != -1) {
                if (tags[tag] != -1) {
                    printfError("duplicate tag\n");
                    return -1;
                } else {
                    tags[tag] = code_ptr - code;
                }
            } else {
                tags[tags_now] = code_ptr - code;
                tags_now++;
            } 
            memset(command_str, '\0', MAX_COMMAND_LEN);
            continue;
        }

        command_t command = parse_command(command_str);

        char arg[MAX_ARG_LEN] = {};
        int shift = 0;
        long arg_l = 0;

        switch (command) {
	        case POP:
	        case PUSH:
	        {
	            GET_ARG("PUSH");

	            char *arg_ptr = arg;

                if (*arg_ptr == '[') {
                    command = (command_t)(command | RAM_MASK);
                    arg_ptr++;
                }

                if (isalpha(*arg_ptr)) {
                    command = (command_t)(command | REG_MASK);

                    *code_ptr = (uint8_t)command;
	                code_ptr++;

	                arg_l = get_register_number(arg_ptr);
	                if (arg_l == -1) {
	                    printfError("unknown register %s\n", arg_ptr);
	                    return 1;
	                }
                } else {
                    *code_ptr = (uint8_t)command;
	                code_ptr++;

	                arg_l = strtol(arg_ptr, NULL, 10);
                }
                
	            PUT_ARG(code_ptr, arg_l);
	            break;
	        }
	        case ADD:
	        case SUB:
	        case MUL:
	        case DIV:
	        case OUT:
	        case RET:
	        case HLT:
	        case VOUT:
	        case IN:
	            NO_ARG("ADD/SUB/MUL/DIV/OUT/RET");

	            buffer++;
	            *code_ptr = (uint8_t)command;
	            code_ptr++;
	            break;
	        case JMP:
	        case JA:
	        case JB:
	        case JAE:
	        case JBE:
	        case JE:
            case JNE:
            case CALL:
                GET_ARG("JMPS"); 

                hash_tag_now = getHashString(arg);
	            arg_l = get_tag(hash_tag_now, tags_now, hash_tags);

	            *code_ptr = (uint8_t)command;
	            code_ptr++;

	            if (arg_l != -1) {
	                PUT_ARG(code_ptr, arg_l);
	            } else {
	                PUT_ARG(code_ptr, tags_now);
	                hash_tags[tags_now] = getHashString(arg);
	                tags_now++;
	            }

	           break;
	        case UNKNOWN_COMMAND:
	        default:
	            printfError("unknown command\n");
	            return 1;
        }
        
        memset(command_str, '\0', MAX_COMMAND_LEN);
    }
    printfOK("end first compile\n");

    for (uint8_t *i = code; i < code_ptr; i++) {
        // printf("i: %ld\n", *i);

        long tag2fix = 0;
        switch ((*i) & OPCODE_MASK) {
            case JMP:
	        case JA:
	        case JB:
	        case JAE:
	        case JBE:
	        case JE:
            case JNE:
            case CALL:
                i++;
                memcpy(&tag2fix, i, sizeof(tags[0]));
                if (tags[tag2fix] == -1) {
                    printfError("undefined tag index %ld\n", tag2fix);
                    return 1;
                }
                PUT_ARG(i, tags[tag2fix]);
                i--;
                break;
	        case PUSH:
	        case POP:
	            i += sizeof(long);
	            break;
	        default:
	            break;
        }
    }
 
    fwrite(code, sizeof(uint8_t), code_ptr - code, file);

    DEBUG {
        for (size_t i = 0; i < code_ptr - code; i++) {
            printf("%d ", code[i]);
        }
        printf("\n");
    }
    printfOK("end second compile\n");

    free(real_buffer);
    fclose(file);
    return 0;
}

int get_tag(const uint64_t tag, const long tags_now, uint64_t *hash_tags) {
    assert(tag);
    assert(hash_tags);

    for (int i = 0; i < tags_now; i++) {
        if (tag == hash_tags[i]) {
            return i;
        }
    }

    return -1;
}

command_t parse_command(const char *command_now) {
    assert(command_now);

    uint64_t hash = getHash8char(command_now);

    for (int i = 0; i < N_COMMANDS; i++) {
        if (hash == hash_spu_commands[i]) {
            return list_spu_commands[i];
        }
    }
    return UNKNOWN_COMMAND;
}

long get_register_number(const char* arg) {
    assert(arg);

    if (arg[1] == 'X') return arg[0] - 'A';
    return -1;

    // if (!strncmp(arg, "AX", 2)) return 0;
    // if (!strncmp(arg, "BX", 2)) return 1;
    // if (!strncmp(arg, "CX", 2)) return 2;
    // if (!strncmp(arg, "DX", 2)) return 3;
    // if (!strncmp(arg, "EX", 2)) return 4;
    // if (!strncmp(arg, "FX", 2)) return 5;
    // if (!strncmp(arg, "GX", 2)) return 6;
    // if (!strncmp(arg, "HX", 2)) return 7;
    //
    // return -1;
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
    assert(comment_ptr);

    while (*comment_ptr != '\0') {
        while (*comment_ptr != ';'  && *comment_ptr != '\0') comment_ptr++;
        while (*comment_ptr != '\n' && *comment_ptr != '\0') {
            *comment_ptr = ' ';
            comment_ptr++;
        }
    }
}


