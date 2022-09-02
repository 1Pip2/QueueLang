#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"
#include "fetch.h"
#include "errors.h"
#include "queue.h"

void setOption(VmOptions* options, char* cmd) {
    if (strcmp(cmd, "-info") == 0) {
        options->dumpInfo = 1;
    } else {
        printf("Error: Unknown option '%s'\n", cmd);
        RAISE_COMMON();
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: qli [option] file\n");
        exit(ERR_IO);
    }

    VmOptions* options = malloc(sizeof(VmOptions));
    *options = (VmOptions) {0};
    for (size_t i = 1; i < argc - 1; i++) {
        setOption(options, argv[i]);
    }

    u_int8_t* code = fetchCode(argv[argc - 1]);
    vmInterpret(code, options);
}
