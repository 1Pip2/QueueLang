#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "compiler.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: qlc file\n");
        exit(1);
    }

    TkQueue* tokens = lexFile(argv[1]);
    compileTokens(tokens, argv[1]);
}
