#include <stdio.h>
#include <stdlib.h>

#include "op.h"
#include "builtins.h"
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

void execOr(VirtMachine* vm) {
    CONDOP(||);
}
void execXor(VirtMachine* vm) {
    CONDOP(!=);
}
void execAnd(VirtMachine* vm) {
    CONDOP(&&);
}
void execNot(VirtMachine* vm) {
    Qitem* op = dequeue(vm->queue);
    expectQitemDt(vm, op, BOOLDT);

    VmData* new = gcMalloc(vm->gc, sizeof(VmData));
    new->type = BOOLDT;
    new->data = op->data->data == 0;
    enqueue(vm->queue, new, VMOP_DATA);
    free(op);
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

    Qitem* vardata = dequeue(vm->queue);
    if (vm->vars[index]->present == 0) {
        vm->vars[index]->present = 1;
        vm->vars[index]->writeable = writeable;
    } else {
         if (vm->vars[index]->writeable == 0) {
            dumpQueue(vm->queue);
            printf("TypeError: Cannot 'set' constant variable\n");
            RAISE_TYPE();
         }

         expectQitemDt(vm, vardata, vm->vars[index]->data->type);
         if (vm->vars[index]->writeable != writeable) {
            dumpQueue(vm->queue);
            printf("TypeError: Cannot convert var to constant\n");
            RAISE_TYPE();
         } 
    }
        
    if (vardata->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        RAISE_INVALID_ARG();
    }

    vm->vars[index]->data = vardata->data;

    free(vardata);
}

void execCall(VirtMachine* vm, u_int64_t data) {
    switch (data) {
    case BUILTIN_GET:
        builtinGet(vm);
        break;
    case BUILTIN_APPEND:
        builtinAppend(vm);
        break;
    case BUILTIN_POP:
        builtinPop(vm);
        break;
    case BUILTIN_SIZE:
        builtinSize(vm);
        break;
    
    default:
        dumpQueue(vm->queue);
        RAISE_UNREACHABLE();
    }
}

void execCallc(VirtMachine* vm, u_int64_t data, u_int8_t* code) {
    vmInterpret(code + data, vm->options);
}

void execExit(VirtMachine* vm) {
    Qitem* retval = dequeue(vm->queue);
    expectQitemDt(vm, retval, INTDT);
    exit(retval->data->data);
}
