#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "errors.h"
#include "queue.h"

// TODO: Proper TypeError
#define EXPECT_DT(qitem, expected_type) \
if (qitem->type != VMOP_DATA) { \
    RAISE_INVALID_ARG(); \
}\
if (qitem->data->type != expected_type) { \
    RAISE_UNREACHABLE(); \
}

#define MATHOP(operation) \
Qitem* op1 = dequeue(vm->queue); \
Qitem* op2 = dequeue(vm->queue); \
\
EXPECT_DT(op1, VMDT_INT); \
EXPECT_DT(op2, VMDT_INT); \
\
VmData* new = malloc(sizeof(VmData)); \
new->type = VMDT_INT; \
new->data = op1->data->data operation op2->data->data; \
enqueue(vm->queue, new, VMOP_DATA); \
\
free(op1->data); \
free(op2->data); \
free(op1); \
free(op2);

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

void execDump(VirtMachine* vm) {
    Qitem* printval = dequeue(vm->queue);
    EXPECT_DT(printval, VMDT_INT);
    printf("%ld\n", (int64_t) printval->data->data);
    free(printval->data);
    free(printval);
}

void execExit(VirtMachine* vm) {
    Qitem* retval = dequeue(vm->queue);
    EXPECT_DT(retval, VMDT_INT);
    exit(retval->data->data);
} 

static inline void opData(VirtMachine* vm) {
    VmData* new = malloc(sizeof(VmData));
    enqueue(vm->queue, new, VMOP_DATA);
    new->type = *(vm->ip++);
    new->data = *((u_int64_t*) vm->ip);
    vm->ip += 8;
}

void opDo(VirtMachine* vm) {
    Qitem* op = dequeue(vm->queue);
    switch (op->type) {
    case VMOP_ADD:
        execAdd(vm);
        break;
    case VMOP_SUB:
        execSub(vm);
        break;
    case VMOP_MULT:
        execMult(vm);
        break;
    case VMOP_DIV:
        execDiv(vm);
        break;
    case VMOP_MOD:
        execMod(vm);
        break;

    case VMOP_DUMP:
        execDump(vm);
        break;

    case VMOP_EXIT:
        execExit(vm);
        break;

    case VMOP_DATA:
        RAISE_EXEC_DATA();
    case VMOP_DO:
        RAISE_UNREACHABLE();
    default:
        RAISE_UNREACHABLE();
    }

    free(op);
}

VirtMachine* vmInit(u_int8_t* code) {
    VirtMachine* new = malloc(sizeof(VirtMachine));
    new->ip = code;
    new->queue = queueInit();
    return new;
}

_Noreturn void vmInterpret(u_int8_t* code) {
    VirtMachine* vm = vmInit(code);

    VmOp op;
    while (1) {
        op = *(vm->ip++);
        if (op == VMOP_DATA) {
            opData(vm);

        } else if (op == VMOP_DO) {
            opDo(vm);
        
        } else if (op >= VMOP_ADD && op <= VMOP_EXIT) {    
            enqueue(vm->queue, NULL, op);

        } else {
            RAISE_UNREACHABLE();
        }
    }    
}
