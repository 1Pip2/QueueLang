#include <stdio.h>
#include <stdlib.h>

#include "gc.h"
#include "vm.h"
#include "errors.h"
#include "queue.h"

GarbageCollector gc[1];
void gcInit(void) {
    gc->head = NULL;
    gc->tail = NULL;
}

HeapFragment* appendPointer(HeapFragment* heapfrag, void* ptr) {
    heapfrag->marked = 0;
    heapfrag->ptr = ptr;
    heapfrag->next = NULL;
    if (gc->head == NULL) {
        gc->head = heapfrag;
        gc->tail = heapfrag;
    } else {
        gc->tail->next = heapfrag;
        gc->tail = heapfrag;
    }

    return heapfrag;
}

void removePointer(HeapFragment* remove, HeapFragment* last) {
    if (last == NULL) {
        gc->head = remove->next;
    } else {
        last->next = remove->next;
    }

    if (remove == gc->tail) {
        gc->tail = last;
    }
}

void* gcMalloc(size_t size) {
    void* ptr = malloc(size);
    HeapFragment* curr = appendPointer(malloc(sizeof(HeapFragment)), ptr);

    return ptr;
}

void* gcRealloc(void* ptr, size_t size) {
    HeapFragment* curr = gc->head;
    while (curr != NULL) {
        if (curr->ptr == ptr) {
            break;
        }

        curr = curr->next;
    }

    if (curr == NULL) {
        curr = appendPointer(malloc(sizeof(HeapFragment)), NULL);
    }

    ptr = realloc(ptr, size);
    curr->ptr = ptr;
    return ptr;
}

void markPtr( void* ptr) {
    HeapFragment* curr;
    for (curr = gc->head; curr != NULL; curr = curr->next) {
        if (curr->ptr == ptr) {
            curr->marked = 1;
            return;
        }
    }

    printf("FatalError: Unreachable: Pointer %p was not allocated\n", ptr);
    exit(ERR_FATAL);
}

void markArrayData(u_int64_t data, VmDataType type) {
    if (type.array_deph > 0) {
        VmArray* array = (void*) data;
        for (size_t i = 0; i < array->size; i++) {
            markArrayData(array->values[i], (VmDataType) {type.type, type.array_deph - 1});
        }

        if (array->size != 0) {
            markPtr(array->values);
        }
        markPtr(array);
    }
}

void markData(VmData* data) {
    if (data->type.array_deph > 0) {
        VmArray* array = (void*) data->data;
        for (size_t i = 0; i < array->size; i++) {
            markArrayData(array->values[i], (VmDataType) {data->type.type, data->type.array_deph - 1});
        }

        if (array->size != 0) {
            markPtr(array->values);
        }
        markPtr(array);

    }

    markPtr(data);
}

void markQueue(Queue* queue) {
    Qitem* curr;
    for (curr = queue->front; curr != NULL; curr = curr->last) {
        if (curr->type == VMOP_DATA) {
            markData((void*) curr->data);
        }
    }
}

void markVars(VmVar** vars, size_t var_num) {
    for (size_t i = 0; i < var_num; i++) {
        if (vars[i]->present) {
            markData(vars[i]->data);
        }
    }
}

void sweep(void) {
    HeapFragment* last = NULL;
    HeapFragment* curr = gc->head;
    while (curr != NULL) {
        if (curr->marked == 0) {
            removePointer(curr, last);

            free(curr->ptr);
            free(curr);
            if (last == NULL) {
                curr = gc->head;
            } else {
                curr = last;
                curr = curr->next;
            }
        } 
        else {
            curr->marked = 0;
            last = curr;
            curr = curr->next;
        }
    }
}
