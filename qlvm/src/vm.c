#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "op.h"
#include "gc.h"
#include "data.h"
#include "errors.h"
#include "queue.h"


void opData(VirtMachine* vm) {
    enqueue(vm->queue, initData(vm), VMOP_DATA);
}

void opVar(VirtMachine* vm) {
    u_int64_t index = *((u_int64_t*) vm->ip);
    vm->ip += 8;

    if (vm->var_num <= index) {
        printf("Error: Undefined variable\n");
        RAISE_COMMON();
    } else if (vm->vars[index]->present == 0) {
        printf("Error: Undefined variable\n");
        RAISE_COMMON();
    }

    enqueue(vm->queue, vm->vars[index]->data, VMOP_DATA);
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

    case VMOP_SET:
        execSet(vm, 1, (u_int64_t) op->data);
        break;
    case VMOP_LET:
        execSet(vm, 0, (u_int64_t) op->data);
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

void opJmp(VirtMachine*, u_int8_t*);
void opJmpNif(VirtMachine* vm, u_int8_t* base) {
    Qitem* bool = dequeue(vm->queue);
    expectQitemDt(vm, bool, BOOLDT);
    if (bool->data->data == 0) {
        opJmp(vm, base);
    } else {
        vm->ip += 8;
    }
}

void opJmp(VirtMachine* vm, u_int8_t* base) {
    u_int64_t offset = *((u_int64_t*) vm->ip);
    vm->ip = base + offset;
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
    enqueue(vm->queue, front->data, front->type);
}

void opCpy(VirtMachine* vm) {
    Qitem* front = queuePeek(vm->queue);
    VmData* data = NULL;
    if (front->type == VMOP_DATA) {
        data = copyData(front->data->type, front->data->data);
    }

    enqueue(vm->queue, data, front->type);
}

void opRm(VirtMachine* vm) {
    free(dequeue(vm->queue));
}

void opSet(VirtMachine* vm, VmOp op) {
    enqueue(vm->queue, (void*) *((u_int64_t*) vm->ip), op);
    vm->ip += 8;
    vm->op_count++;
}

VirtMachine* vmInit(u_int8_t* code) {
    VirtMachine* new = malloc(sizeof(VirtMachine));
    new->ip = code;
    new->queue = queueInit();
    new->op_count = 0;

    new->vars = NULL;
    new->var_num = 0;
    return new;
}

_Noreturn void vmInterpret(u_int8_t* code, VmOptions* options) {
    VirtMachine* vm = vmInit(code);
    gcInit();

    VmOp op;
    while (1) {
        if (options->dumpInfo) {
            dumpQueue(vm->queue);
        }

        markQueue(vm->queue);
        markVars(vm->vars, vm->var_num);
        sweep();
        
        op = *(vm->ip++);
        if (op == VMOP_DATA) {
            opData(vm);
        } else if (op == VMOP_VAR) {
            opVar(vm);

        } else if (op == VMOP_DO) {
            opDo(vm);
            vm->op_count--;
        } else if (op == VMOP_DOALL) {
            for (; vm->op_count > 0; vm->op_count--) {
                opDo(vm);
            }

        } else if (op == VMOP_JMPNIF) {
            opJmpNif(vm, code);
        } else if (op == VMOP_JMP) {
            opJmp(vm, code);

        } else if (op == VMOP_REQ) { 
            opReq(vm);

        } else if (op == VMOP_DUP) { 
            opDup(vm);
        } else if (op == VMOP_CPY) { 
            opCpy(vm);
        } else if (op == VMOP_RM) { 
            opRm(vm);

        } else if (op == VMOP_SET || op == VMOP_LET) { 
            opSet(vm, op);

        } else if (op >= VMOP_ADD && op <= VMOP_EXIT) {    
            enqueue(vm->queue, NULL, op);
            vm->op_count++;

        } else {
            dumpQueue(vm->queue);
            RAISE_UNREACHABLE();
        }
    }    
}
