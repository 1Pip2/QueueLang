#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "errors.h"
#include "queue.h"

// TODO: Proper TypeError
#define EXPECT_DT(qitem, expected_type) \
if (qitem->type != VMOP_DATA) { \
    dumpQueue(vm->queue); \
    RAISE_INVALID_ARG(); \
}\
if (qitem->data->type != expected_type) { \
    dumpQueue(vm->queue); \
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

#define COMPOP(operation) \
Qitem* op1 = dequeue(vm->queue); \
Qitem* op2 = dequeue(vm->queue); \
\
EXPECT_DT(op1, VMDT_INT); \
EXPECT_DT(op2, VMDT_INT); \
\
VmData* new = malloc(sizeof(VmData)); \
new->type = VMDT_BOOL; \
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

    switch (printval->data->type) {
    case VMDT_INT:
        printf("%ld\n", (int64_t) printval->data->data);
        break;
    case VMDT_BOOL:
        if (printval->data->data) {
            puts("True");
        } else {
            puts("False");
        }
        break;
    
    default:
        dumpQueue(vm->queue);
        RAISE_UNREACHABLE();
    }

    free(printval->data);
    free(printval);
}


void execExit(VirtMachine* vm) {
    Qitem* retval = dequeue(vm->queue);
    EXPECT_DT(retval, VMDT_INT);
    exit(retval->data->data);
} 

void opData(VirtMachine* vm) {
    VmData* new = malloc(sizeof(VmData));
    enqueue(vm->queue, new, VMOP_DATA);
    new->type = *(vm->ip++);
    switch (new->type) {
    case VMDT_INT:
        new->data = *((u_int64_t*) vm->ip);
        vm->ip += 8;
        break;
    case VMDT_BOOL:
        new->data = *(vm->ip++);
        break;
    
    default:
        dumpQueue(vm->queue);
        RAISE_UNREACHABLE();
        break;
    }
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

    case VMOP_EQU:
        execEqu(vm);
        break;
    case VMOP_GREATER:
        execGreater(vm);
        break;
    case VMOP_LESS:
        execLess(vm);
        break;
    case VMOP_GREATEREQU:
        execGreaterEqu(vm);
        break;
    case VMOP_LESSEQU:
        execLessEqu(vm);
        break;

    case VMOP_DUMP:
        execDump(vm);
        break;

    case VMOP_EXIT:
        execExit(vm);
        break;

    case VMOP_DATA:
        dumpQueue(vm->queue);
        RAISE_EXEC_DATA();
    case VMOP_DO:
        dumpQueue(vm->queue);
        RAISE_UNREACHABLE();
    default:
        dumpQueue(vm->queue);
        RAISE_UNREACHABLE();
    }

    free(op);
}

void opReq(VirtMachine* vm) {
    Qitem* front = dequeue(vm->queue);
    front->last = NULL;

    if (vm->queue->back == NULL) {
        vm->queue->front = front;
        vm->queue->back = front;
    } else {
        vm->queue->back->last = front;
        vm->queue->back = front;
    }
}
void opDup(VirtMachine* vm) {
    Qitem* front = queuePeek(vm->queue);
    VmData* data = NULL;
    if (front->type == VMOP_DATA) {
        data = malloc(sizeof(VmData));
        data->type = front->data->type;
        data->data = front->data->data;
    }
    enqueue(vm->queue, data, front->type);
}

VirtMachine* vmInit(u_int8_t* code) {
    VirtMachine* new = malloc(sizeof(VirtMachine));
    new->ip = code;
    new->queue = queueInit();
    return new;
}

_Noreturn void vmInterpret(u_int8_t* code, VmOptions* options) {
    VirtMachine* vm = vmInit(code);

    VmOp op;
    while (1) {
        if (options->dumpInfo) {
            dumpQueue(vm->queue);
        }

        op = *(vm->ip++);
        if (op == VMOP_DATA) {
            opData(vm);

        } else if (op == VMOP_DO) {
            opDo(vm);
            vm->op_count--;
        } else if (op == VMOP_DOALL) {
            for (; vm->op_count > 0; vm->op_count--) {
                opDo(vm);
            }
        } else if (op == VMOP_REQ) { 
            opReq(vm);

        } else if (op == VMOP_DUP) { 
            opDup(vm);

        } else if (op >= VMOP_ADD && op <= VMOP_EXIT) {    
            enqueue(vm->queue, NULL, op);
            vm->op_count++;

        } else {
            dumpQueue(vm->queue);
            RAISE_UNREACHABLE();
        }
    }    
}
