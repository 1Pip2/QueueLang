#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "queue.h"

int main(int argc, char** argv) {
    u_int8_t code[] = {
    VMOP_SUB,
    VMOP_DATA, VMDT_INT, 17, 0, 0, 0, 0, 0, 0, 0,  
    VMOP_DATA, VMDT_INT, 10, 0, 0, 0, 0, 0, 0, 0,
    VMOP_MULT,
    VMOP_DATA, VMDT_INT, 4, 0, 0, 0, 0, 0, 0, 0,  
    VMOP_DATA, VMDT_INT, 5, 0, 0, 0, 0, 0, 0, 0,
    VMOP_MOD,

    VMOP_DO,
    VMOP_DO,

    VMOP_DUMP,
    VMOP_DO,
    VMOP_DO,

    VMOP_EXIT,
    VMOP_DATA, VMDT_INT, 0, 0, 0, 0, 0, 0, 0, 0,
    VMOP_DO,
    };
    
    vmInterpret(code);
}
