#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "fetch.h"
#include "errors.h"
#include "queue.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: qli file\n");
        exit(ERR_IO);
    }

    u_int8_t* code = fetchCode(argv[1]);
    vmInterpret(code);
}
