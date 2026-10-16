#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"

void clearInputBuffer() {
    int ch = '\0';
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

bool isZero(double n) { return fabs(n) < EPS; }

bool isEqual(double a, double b) { return isZero(a - b); }

double randDouble() { return (double)rand() / (rand() + 1); }

uint64_t getHash(const void *buffer, const size_t size) {
    assert(buffer);

    const uint8_t *buf = (const uint8_t*)buffer;
    uint64_t ans = 5381;
    for (size_t i = 0; i < size; i++) {
        ans ^= (uint64_t)buf[i];
        ans *= 33;
    }
    return ans;
}

ssize_t readFileToBuffer(const char *filename, char **buffer) {
    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == FS_ERROR) {
        return FS_ERROR;
    }

    ssize_t file_size_return = getFileSize(file_descriptor);
    if (file_size_return == FS_ERROR) {
        return FS_ERROR;
    }

    size_t file_size = (size_t)file_size_return;

    char* pointer = (char*)calloc(file_size + 2, sizeof(char)); // +2 для \n\0 в конце
    if (pointer == NULL) {
        return 0;
    }
    
    ssize_t read_return = read(file_descriptor, pointer, file_size);
    
    if (read_return == FS_ERROR) {
        return FS_ERROR;
    }

    pointer[(size_t)read_return]     = '\n';
    pointer[(size_t)read_return + 1] = '\0';
    
    close(file_descriptor);
    *buffer = pointer;
    return read_return;
}

ssize_t getFileSize(int file_descriptor) {
    struct stat st = {};
    int fstat_return = fstat(file_descriptor, &st);
    if (fstat_return == FS_ERROR) {
        return FS_ERROR;
    }
    return st.st_size;
}

size_t parseBuffer(char *buffer, const size_t buffer_len, String **arr) {
    size_t arr_len = 0;
    for (size_t i = 0; i < buffer_len; i++) if (buffer[i] == '\n') arr_len++;

    String* pointer = (String*)calloc(arr_len, sizeof(String));
    
    if (pointer == NULL) return 0;

    size_t last = 0;
    size_t n_string = 0;
    for (size_t i = 0; i < buffer_len; i++) {
        if (buffer[i] == '\n') {
            pointer[n_string].str = buffer + last;
            pointer[n_string].len = i - last;
            n_string++;
            last = i + 1;
        }
    }

    *arr = pointer;
    
    return arr_len;
}
