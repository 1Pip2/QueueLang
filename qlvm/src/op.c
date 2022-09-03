#include <stdio.h>
#include <stdlib.h>

#include "op.h"
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


void execExit(VirtMachine* vm) {
    Qitem* retval = dequeue(vm->queue);
    expectQitemDt(vm, retval, INTDT);
    exit(retval->data->data);
} 
