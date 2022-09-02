#ifndef QUEUE_H
#define QUEUE_H

#include "vm.h"

/* element in queue */
typedef struct Qitem {
    VmData* data;
    enum VmOp type;

    struct Qitem* last;
} Qitem;

/* queue data structure for vm */
typedef struct Queue {
    Qitem* front;
    Qitem* back;
} Queue;

Queue* queueInit(void);
Qitem* enqueue(Queue*, VmData*, VmOp);
Qitem* dequeue(Queue*);
Qitem* queuePeek(Queue*);
void dumpQueue(Queue*);

#endif
