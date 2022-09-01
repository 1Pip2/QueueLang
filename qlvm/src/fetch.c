#include <stdio.h>
#include <stdlib.h>

#include "fetch.h"
#include "errors.h"

typedef struct Code {
    u_int8_t* code;
    size_t size;
} Code;

void appendByte(Code* code, u_int8_t byte) {
    code->code = realloc(code->code, (++code->size) * sizeof(u_int8_t));
    code->code[code->size - 1] = byte;
}

u_int8_t* fetchCode(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        RAISE_FOPEN(filename);
    }

    Code* code = malloc(sizeof(Code));
    code->code = NULL;
    code->size = 0;

    int curr;
    while ((curr = fgetc(fp)) != EOF) {
        appendByte(code, (u_int8_t) curr);
    }

    fclose(fp);
    u_int8_t* tmp = code->code;
    free(code);
    return tmp;
}
