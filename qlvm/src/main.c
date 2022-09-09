#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"
#include "fetch.h"
#include "errors.h"

void setOption(VmOptions* options, char* cmd) {
    if (strcmp(cmd, "-queue") == 0) {
        options->dumpQueue = 1;
    } else {
        printf("Unknown option '%s'\n", cmd);
        RAISE_USAGE();
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        RAISE_USAGE();
    }

    VmOptions* options = malloc(sizeof(VmOptions));
    *options = (VmOptions) {0};
    for (size_t i = 1; i < argc - 1; i++) {
        setOption(options, argv[i]);
    }

    u_int8_t* code = fetchCode(argv[argc - 1]);
    vmInterpret(code, options);
}
