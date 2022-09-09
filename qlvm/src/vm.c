#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "op.h"
#include "gc.h"
#include "data.h"
#include "errors.h"
#include "queue.h"


void opData(VmFun* fun) {
    enqueue(fun->queue, initData(fun), VMOP_DATA);
}

void opVar(VmFun* fun) {
    u_int64_t index = *((u_int64_t*) fun->ip);
    fun->ip += 8;
    
    if (fun->var_num <= index) {
        RAISE_NAME_ERROR(fun->queue);
    } else if (fun->vars[index]->present == 0) {
        RAISE_NAME_ERROR(fun->queue);
    }

    enqueue(fun->queue, fun->vars[index]->data, VMOP_DATA);
}

void opArgNum(VmFun* fun) {
    fun->var_num = *((u_int64_t*) fun->ip);
    fun->vars = realloc(fun->vars, fun->var_num * sizeof(VmVar*));
    for (size_t i = 0; i < fun->var_num; i++) {
        Qitem* vardata = dequeue(fun->ret->queue);
        fun->vars[i] = malloc(sizeof(VmVar));
        fun->vars[i]->present = 1;
        fun->vars[i]->writeable = 1;
            
        if (vardata->type != VMOP_DATA) {
            RAISE_EXPECT_DATA(fun->queue);
        }

        fun->vars[i]->data = vardata->data;
        free(vardata);
    }

    fun->ip += 8;
}

void opDo(VirtMachine* vm) {
    Qitem* op = dequeue(vm->curr_fun->queue);
    switch (op->type) {
    case VMOP_ADD:
        execAdd(vm->curr_fun);
        break;
    case VMOP_SUB:
        execSub(vm->curr_fun);
        break;
    case VMOP_MULT:
        execMult(vm->curr_fun);
        break;
    case VMOP_DIV:
        execDiv(vm->curr_fun);
        break;
    case VMOP_MOD:
        execMod(vm->curr_fun);
        break;

    case VMOP_EQU:
        execEqu(vm->curr_fun);
        break;
    case VMOP_GREATER:
        execGreater(vm->curr_fun);
        break;
    case VMOP_LESS:
        execLess(vm->curr_fun);
        break;
    case VMOP_GREATEREQU:
        execGreaterEqu(vm->curr_fun);
        break;
    case VMOP_LESSEQU:
        execLessEqu(vm->curr_fun);
        break;

    case VMOP_OR:
        execOr(vm->curr_fun);
        break;
    case VMOP_XOR:
        execXor(vm->curr_fun);
        break;
    case VMOP_AND:
        execAnd(vm->curr_fun);
        break;
    case VMOP_NOT:
        execNot(vm->curr_fun);
        break;

    case VMOP_DUMP:
        execDump(vm->curr_fun);
        break;

    case VMOP_SET:
        execSet(vm->curr_fun, 1, (u_int64_t) op->data);
        break;
    case VMOP_LET:
        execSet(vm->curr_fun, 0, (u_int64_t) op->data);
        break;
    case VMOP_CALL:
        execCall(vm->curr_fun, (u_int64_t) op->data);
        break;
    case VMOP_CALLC:
        execCallc(vm, (u_int64_t) op->data);
        break;

    case VMOP_DATA:
        RAISE_EXECUTION_TYPE(vm->curr_fun->queue);
    default:
        RAISE_UNREACHABLE(vm->curr_fun->queue);
    }

    free(op);
}

void opJmp(VirtMachine*);
void opJmpNif(VirtMachine* vm) {
    Qitem* bool = dequeue(vm->curr_fun->queue);
    expectQitemDt(vm->curr_fun, bool, BOOLDT);
    if (bool->data->data == 0) {
        opJmp(vm);
    } else {
        vm->curr_fun->ip += 8;
    }

    free(bool);
}

void opJmp(VirtMachine* vm) {
    u_int64_t offset = *((u_int64_t*) vm->curr_fun->ip);
    vm->curr_fun->ip = vm->code + offset;
}

void opReq(VmFun* fun) {
    Qitem* front = dequeue(fun->queue);
    front->last = NULL;

    if (fun->queue->back == NULL) {
        fun->queue->front = front;
        fun->queue->back = front;
    } else {
        fun->queue->back->last = front;
        fun->queue->back = front;
    }

    free(front);
}

void opDup(VmFun* fun) {
    Qitem* front = queuePeek(fun->queue);
    enqueue(fun->queue, front->data, front->type);
}

void opCpy(VmFun* fun) {
    Qitem* front = queuePeek(fun->queue);
    VmData* data = NULL;
    if (front->type == VMOP_DATA) {
        data = copyData(fun, front->data->type, front->data->data);
    }

    enqueue(fun->queue, data, front->type);
}

void opRm(VmFun* fun) {
    free(dequeue(fun->queue));
}

void opOpAndArg(VmFun* fun, VmOp op) {
    enqueue(fun->queue, (void*) *((u_int64_t*) fun->ip), op);
    fun->ip += 8;
    fun->op_count++;
}

void opRet(VirtMachine* vm) {
    if (vm->curr_fun->ret == NULL) {
        exit(0);
    }

    for (size_t i = 0; i < vm->curr_fun->var_num; i++) {
        free(vm->curr_fun->vars[i]);
    }
    if (vm->curr_fun->vars != NULL) {
        free(vm->curr_fun->vars);
    }

    markQueue(vm->curr_fun->gc, vm->curr_fun->queue);
    sweep(vm->curr_fun->gc);
    if (vm->curr_fun->ret->gc->tail == NULL) {
        vm->curr_fun->ret->gc->head = vm->curr_fun->gc->head;
        vm->curr_fun->ret->gc->tail = vm->curr_fun->gc->tail;
    } else {
        vm->curr_fun->ret->gc->tail->next = vm->curr_fun->gc->head;
        vm->curr_fun->ret->gc->tail = vm->curr_fun->gc->tail;
    }
    free(vm->curr_fun->gc);

    Qitem* tmp;
    for (Qitem* curr = vm->curr_fun->queue->front; curr != NULL; free(tmp)) {
        tmp = curr;
        curr = curr->last;
        enqueue(vm->curr_fun->ret->queue, tmp->data, tmp->type);
    }
    free(vm->curr_fun->queue);

    VmFun* fun = vm->curr_fun;
    vm->curr_fun = vm->curr_fun->ret;
    free(fun);
}

VmFun* vmFunInit(VmFun* ret, u_int8_t* ip) {
    VmFun* fun = malloc(sizeof(VmFun));
    fun->ip = ip;
    fun->queue = queueInit();
    fun->gc = gcInit();

    fun->vars = NULL;
    fun->var_num = 0;
    
    fun->op_count = 0;
    fun->ret = ret;
    return fun;
}

VirtMachine* vmInit(u_int8_t* code, VmOptions* options) {
    VirtMachine* new = malloc(sizeof(VirtMachine));
    new->code = code;
    new->curr_fun = vmFunInit(NULL, code);
    new->options = options;
    return new;
}

void vmInterpret(u_int8_t* code, VmOptions* options) {
    VirtMachine* vm = vmInit(code, options);

    VmOp op;
    VmFun* curr;
    while (1) {
        if (vm->options->dumpQueue) {
            dumpQueue(vm->curr_fun->queue);
        }

        curr = vm->curr_fun;
        markQueue(curr->gc, curr->queue);
        markVars(curr->gc, curr->vars, curr->var_num);
        sweep(curr->gc);
        
        op = *(curr->ip++);
        if (op == VMOP_DATA) {
            opData(curr);
        } else if (op == VMOP_VAR) {
            opVar(curr);
        } else if (op == VMOP_ARGNUM) {
            opArgNum(curr);

        } else if (op == VMOP_DO) {
            opDo(vm);
            curr->op_count--;
        } else if (op == VMOP_DOALL) {
            if (curr->op_count > 0) {
                curr->op_count--;
                curr->ip--;  // loop
                opDo(vm);
            }

        } else if (op == VMOP_JMPNIF) {
            opJmpNif(vm);
        } else if (op == VMOP_JMP) {
            opJmp(vm);

        } else if (op == VMOP_REQ) { 
            opReq(curr);

        } else if (op == VMOP_DUP) { 
            opDup(curr);
        } else if (op == VMOP_CPY) { 
            opCpy(curr);
        } else if (op == VMOP_RM) { 
            opRm(curr);
        } else if (op == VMOP_RET) {
            opRet(vm);

        } else if (op == VMOP_SET || op == VMOP_LET || op == VMOP_CALL || op == VMOP_CALLC) { 
            opOpAndArg(curr, op);

        } else if (op >= VMOP_ADD && op <= VMOP_RET - 1) {    
            enqueue(curr->queue, NULL, op);
            curr->op_count++;

        } else {
            RAISE_UNREACHABLE(curr->queue);
        }
    }    
}
