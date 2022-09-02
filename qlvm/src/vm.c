#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "op.h"
#include "data.h"
#include "errors.h"
#include "queue.h"


void opData(VirtMachine* vm) {
    enqueue(vm->queue, initData(vm), VMOP_DATA);
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
        data = copyData(front->data);
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
