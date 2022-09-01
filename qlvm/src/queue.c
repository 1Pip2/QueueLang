#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
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
