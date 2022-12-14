#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "op.h"
#include "data.h"
#include "errors.h"
#include "queue.h"

Queue* queueInit(void) {
    Queue* new = malloc(sizeof(Queue));
    new->front = NULL;
    new->back = NULL;
    return new;
}

Qitem* enqueue(Queue* queue, VmData* data, VmOp type) {
    Qitem* new = malloc(sizeof(Qitem));
    new->data = data;
    new->type = type;
    new->last = NULL;
    if (queue->back == NULL) {
        queue->front = new;
        queue->back = new;
        return new;
    }

    queue->back->last = new;
    queue->back = new;
    return new;
}

Qitem* dequeue(Queue* queue) {
    if (queue->front == NULL) {
        RAISE_EMPTY_QUEUE();
    }


    Qitem* tmp = queue->front;
    
    queue->front = queue->front->last;
    if (tmp == queue->back) {  // if queue length == 1
        queue->back = NULL;
    }
    
    return tmp;
}

Qitem* queuePeek(Queue* queue) {
    if (queue->front == NULL) {
        RAISE_EMPTY_QUEUE();
    }


    return queue->front;
}

/* Return string of VmOpType */
char* strQitem(Qitem* op) {
    switch (op->type) {
    case VMOP_DATA:
        return "Data";

    case VMOP_ADD:
        return "+";
    case VMOP_SUB:
        return "-";
    case VMOP_MULT:
        return "*";
    case VMOP_DIV:
        return "/";
    case VMOP_MOD:
        return "%";
    
    case VMOP_EQU:
        return "=";
    case VMOP_GREATER:
        return ">";
    case VMOP_LESS:
        return "<";
    case VMOP_GREATEREQU:
        return ">=";
    case VMOP_LESSEQU:
        return "<=";

    case VMOP_OR:
        return "or";
    case VMOP_XOR:
        return "xor";
    case VMOP_AND:
        return "and";
    case VMOP_NOT:
        return "not";
    
    case VMOP_DUMP:
        return ".";
    case VMOP_SET:
        return "set";
    case VMOP_LET:
        return "let";
    case VMOP_CALL:
        return "call";
    case VMOP_CALLC:
        return "call";
    
    default:
        puts("FatalError: Unreachable(bytecode might be corrupted)");
        exit(ERR_FATAL);
    }
}

/* print queue to stdout */
void dumpQueue(Queue* queue) {
    printf("Queue: [");

    Qitem* curr = queue->front;
    while (curr != NULL) {
        if (curr->type == VMOP_DATA) {
            printDataType(curr->data->type);
            printf("; ");
        } else {
            printf("%s; ", strQitem(curr));
        }

        curr = curr->last;
    }

    printf("]\n");
}
