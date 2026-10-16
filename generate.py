#!/usr/bin/python3

commands = [
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
]

max_comand_len = max([len(i) for i in commands])

with open("commands.h", "w") as file:
    file.write(f"#ifndef _COMMANDS_H\n#define _COMMANDS_H\n\n#include <stdlib.h>\n\n#define MAX_COMMAND_LEN {max_comand_len}\n")
    file.write(f"#define N_COMMANDS {len(commands)}\n\n")
    file.write("typedef enum {\n")
    for i, el in enumerate(commands):
        file.write("\t" + el)
        if i != len(commands) - 1:
            file.write(",\n")
        else:
            file.write("\n")
    file.write("} command_t;\n\n")

    file.write(f"static const char str_spu_commands[{len(commands)}][{max_comand_len + 1}] = " + "{\n")
    for i, el in enumerate(commands):
        file.write('\t"' + el + '"')
        if i != len(commands) - 1:
            file.write(",\n")
        else:
            file.write("\n")
    file.write("};\n\n")

    file.write(f"static const size_t len_spu_commands[{len(commands)}] = " + "{\n\t")
    for i, el in enumerate(commands):
        file.write(str(len(el)))
        if i != len(commands) - 1:
            file.write(", ")
        else:
            file.write("\n")

    file.write("};\n\n")

    file.write(f"static const command_t list_spu_commands[{len(commands)}] = " + "{\n\t")
    for i, el in enumerate(commands):
        file.write(el)
        if i != len(commands) - 1:
            file.write(", ")
        else:
            file.write("\n")

    file.write("};\n")
    file.write("\n#endif\n")

