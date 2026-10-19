#ifndef _COMMON_H
#define _COMMON_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define EPS 1e-9
#define STATIC_STRLEN(s) (sizeof(s) - 1)
#define STATIC_LEN(s) ((int)sizeof(s) / (int)sizeof(s[0]))

#define BLACK     "\033[30m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define BLUE      "\033[34m"
#define PURPLE    "\033[35m"
#define LIGHTBLUE "\033[36m"
#define WHITE     "\033[37m"
#define STANDART  "\033[0m"

#define STRDEF(x) #x
#define STR(x) STRDEF(x)

#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define printfError(mes, ...) printf(  RED  "[ERROR] " STANDART mes, ##__VA_ARGS__);
#define printfOK(mes, ...)    printf(GREEN     "[OK] " STANDART mes, ##__VA_ARGS__);

#define FS_ERROR -1

void clearInputBuffer();

bool isZero (double n);
bool isEqual(double a, double b);

double randDouble();

uint64_t getHash(const void *buffer, const size_t size);

uint64_t getHashString(const char *s);

uint64_t getHash8char(const char *s);

typedef struct {
    char *str;
    size_t len;
} String;

ssize_t readFileToBuffer(const char *filename, char **buffer);

size_t parseBuffer(char *buffer, const size_t buffer_len, String **arr);

ssize_t getFileSize(int file_descriptor);
#endif
