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
    "JNE",
    "CALL",
    "RET"
]


def getHash(s):
    # ans = 5381
    # for i in s:
    #     ans ^= ord(i)
    #     ans *= 33
    # return ans
    return int.from_bytes(s[:8].encode("ASCII"), "little")


max_comand_len = 8  # max([len(i) for i in commands])

with open("commands.h", "w") as file:
    file.write("#ifndef _COMMANDS_H\n")
    file.write("#define _COMMANDS_H\n")
    file.write("\n")
    file.write("#include <stdlib.h>\n")
    file.write("#include <stdint.h>\n")
    file.write("\n")
    file.write(f"#define MAX_COMMAND_LEN {max_comand_len}\n")
    file.write(f"#define N_COMMANDS {len(commands)}\n")
    file.write("\n")
    file.write("typedef enum {\n")
    for i, el in enumerate(commands):
        file.write("\t" + el)
        file.write(",\n")
    file.write("\tUNKNOWN_COMMAND\n")
    file.write("} command_t;\n")
    file.write("\n")

    file.write("static const char str_spu_commands[N_COMMANDS][MAX_COMMAND_LEN + 1] = {\n")
    for i, el in enumerate(commands):
        file.write('\t"' + el + '"')
        if i != len(commands) - 1:
            file.write(",\n")
        else:
            file.write("\n")
    file.write("};\n")
    file.write("\n")

    file.write("static const uint64_t hash_spu_commands[N_COMMANDS] = {\n")
    for i, el in enumerate(commands):
        file.write(f"\t{getHash(el)}")
        if i != len(commands) - 1:
            file.write(",\n")
        else:
            file.write("\n")
    file.write("};\n")
    file.write("\n")

    file.write("static const size_t len_spu_commands[N_COMMANDS] = " + "{\n\t")
    for i, el in enumerate(commands):
        file.write(str(len(el)))
        if i != len(commands) - 1:
            file.write(", ")
        else:
            file.write("\n")

    file.write("};\n")
    file.write("\n")

    file.write("static const command_t list_spu_commands[N_COMMANDS] = {\n\t")
    for i, el in enumerate(commands):
        file.write(el)
        if i != len(commands) - 1:
            file.write(", ")
        else:
            file.write("\n")

    file.write("};\n")
    file.write("\n")
    file.write("#endif\n")

