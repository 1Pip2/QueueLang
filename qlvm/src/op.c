#include <stdio.h>
#include <stdlib.h>

#include "op.h"
#include "gc.h"
#include "data.h"
#include "errors.h"

void execAdd(VirtMachine* vm) {
    MATHOP(+);
}
void execSub(VirtMachine* vm) {
    MATHOP(-);
}
void execMult(VirtMachine* vm) {
    MATHOP(*);
}
void execDiv(VirtMachine* vm) {
    MATHOP(/);
}
void execMod(VirtMachine* vm) {
    MATHOP(%);
}

void execEqu(VirtMachine* vm) {
    COMPOP(==);
}
void execGreater(VirtMachine* vm) {
    COMPOP(>);
}
void execLess(VirtMachine* vm) {
    COMPOP(<);
}
void execGreaterEqu(VirtMachine* vm) {
    COMPOP(>=);
}
void execLessEqu(VirtMachine* vm) {
    COMPOP(<=);
}

void execDump(VirtMachine* vm) {
    Qitem* printval = dequeue(vm->queue);
    if (printval->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        RAISE_INVALID_ARG();
    }

    printData(printval->data->type, printval->data->data);
    puts("");

    free(printval);
}

void execSet(VirtMachine* vm, u_int8_t writeable, u_int64_t index) {
    while (vm->var_num <= index) {
        vm->vars = realloc(vm->vars, (++vm->var_num) * sizeof(VmVar*));
        vm->vars[vm->var_num - 1] = malloc(sizeof(VmVar));
        vm->vars[vm->var_num - 1]->present = 0;
    }

    if (vm->vars[index]->present == 0) {
        vm->vars[index]->present = 1;
        vm->vars[index]->writeable = writeable;
    } else {
         if (vm->vars[index]->writeable == 0) {
            dumpQueue(vm->queue);
            printf("TypeError: Cannot 'set' constant variable\n");
            RAISE_TYPE();
         }
    }
        
    Qitem* vardata = dequeue(vm->queue);
    if (vardata->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected type 'data'\n");
        RAISE_TYPE();
    }

    vm->vars[index]->data = vardata->data;
}

void execExit(VirtMachine* vm) {
    Qitem* retval = dequeue(vm->queue);
    expectQitemDt(vm, retval, INTDT);
    exit(retval->data->data);
}
