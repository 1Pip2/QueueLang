#include <stdio.h>
#include <stdlib.h>

#include "op.h"
#include "builtins.h"
#include "gc.h"
#include "data.h"
#include "errors.h"

void execAdd(VmFun* fun) {
    MATHOP(+);
}
void execSub(VmFun* fun) {
    MATHOP(-);
}
void execMult(VmFun* fun) {
    MATHOP(*);
}
void execDiv(VmFun* fun) {
    MATHOP(/);
}
void execMod(VmFun* fun) {
    MATHOP(%);
}

void execEqu(VmFun* fun) {
    COMPOP(==);
}
void execGreater(VmFun* fun) {
    COMPOP(>);
}
void execLess(VmFun* fun) {
    COMPOP(<);
}
void execGreaterEqu(VmFun* fun) {
    COMPOP(>=);
}
void execLessEqu(VmFun* fun) {
    COMPOP(<=);
}

void execOr(VmFun* fun) {
    CONDOP(||);
}
void execXor(VmFun* fun) {
    CONDOP(!=);
}
void execAnd(VmFun* fun) {
    CONDOP(&&);
}
void execNot(VmFun* fun) {
    Qitem* op = dequeue(fun->queue);
    expectQitemDt(fun, op, BOOLDT);

    VmData* new = gcMalloc(fun->gc, sizeof(VmData));
    new->type = BOOLDT;
    new->data = op->data->data == 0;
    enqueue(fun->queue, new, VMOP_DATA);
    free(op);
}

void execDump(VmFun* fun) {
    Qitem* printval = dequeue(fun->queue);
    if (printval->type != VMOP_DATA) {
        RAISE_EXPECT_DATA(fun->queue);
    }

    printData(printval->data->type, printval->data->data);
    puts("");

    free(printval);
}

void execSet(VmFun* fun, u_int8_t writeable, u_int64_t index) {
    while (fun->var_num <= index) {
        fun->vars = realloc(fun->vars, (++fun->var_num) * sizeof(VmVar*));
        fun->vars[fun->var_num - 1] = malloc(sizeof(VmVar));
        fun->vars[fun->var_num - 1]->present = 0;
    }

    Qitem* vardata = dequeue(fun->queue);
    if (fun->vars[index]->present == 0) {
        fun->vars[index]->present = 1;
        fun->vars[index]->writeable = writeable;
    } else {
         if (fun->vars[index]->writeable == 0) {
            dumpQueue(fun->queue);
            printf("TypeError: Cannot 'set' constant variable\n");
            exit(ERR_TYPE);
         }

         expectQitemDt(fun, vardata, fun->vars[index]->data->type);
         if (fun->vars[index]->writeable != writeable) {
            dumpQueue(fun->queue);
            printf("TypeError: Cannot convert var to constant\n");
            exit(ERR_TYPE);
         } 
    }
        
    if (vardata->type != VMOP_DATA) {
        RAISE_EXPECT_DATA(fun->queue);
    }

    fun->vars[index]->data = vardata->data;

    free(vardata);
}

void execCall(VmFun* fun, u_int64_t data) {
    switch (data) {
    case BUILTIN_GET:
        builtinGet(fun);
        break;
    case BUILTIN_APPEND:
        builtinAppend(fun);
        break;
    case BUILTIN_POP:
        builtinPop(fun);
        break;
    case BUILTIN_SIZE:
        builtinSize(fun);
        break;
    
    default:
        RAISE_UNREACHABLE(fun->queue);
    }
}

void execCallc(VirtMachine* vm, u_int64_t data) {
    vm->curr_fun = vmFunInit(vm->curr_fun, vm->code + data);
}
