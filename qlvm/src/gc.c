#include <stdio.h>
#include <stdlib.h>

#include "gc.h"
#include "vm.h"
#include "errors.h"
#include "queue.h"

GarbageCollector* gcInit(void) {
    GarbageCollector* new = malloc(sizeof(GarbageCollector));
    new->head = NULL;
    new->tail = NULL;
    return new;
}

HeapFragment* appendPointer(GarbageCollector* gc, HeapFragment* heapfrag, void* ptr) {
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

void removePointer(GarbageCollector* gc, HeapFragment* remove, HeapFragment* last) {
    if (last == NULL) {
        gc->head = remove->next;
    } else {
        last->next = remove->next;
    }

    if (remove == gc->tail) {
        gc->tail = last;
    }
}

void* gcMalloc(GarbageCollector* gc, size_t size) {
    void* ptr = malloc(size);
    HeapFragment* curr = appendPointer(gc, malloc(sizeof(HeapFragment)), ptr);

    return ptr;
}

void* gcRealloc(GarbageCollector* gc,void* ptr, size_t size) {
    HeapFragment* curr = gc->head;
    while (curr != NULL) {
        if (curr->ptr == ptr) {
            break;
        }

        curr = curr->next;
    }

    if (curr == NULL) {
        curr = appendPointer(gc, malloc(sizeof(HeapFragment)), NULL);
    }

    ptr = realloc(ptr, size);
    curr->ptr = ptr;
    return ptr;
}

void markPtr(GarbageCollector* gc, void* ptr) {
    HeapFragment* curr;
    for (curr = gc->head; curr != NULL; curr = curr->next) {
        if (curr->ptr == ptr) {
            curr->marked = 1;
            return;
        }
    }

    printf("Pointer %p was not allocated\n", ptr);
    RAISE_UNREACHABLE();
}

void markArrayData(GarbageCollector* gc, u_int64_t data, VmDataType type) {
    if (type.array_deph > 0) {
        VmArray* array = (void*) data;
        for (size_t i = 0; i < array->size; i++) {
            markArrayData(gc, array->values[i], (VmDataType) {type.type, type.array_deph - 1});
        }
        markPtr(gc, array->values);
        markPtr(gc, array);
    }
}

void markData(GarbageCollector* gc, VmData* data) {
    if (data->type.array_deph > 0) {
        VmArray* array = (void*) data->data;
        for (size_t i = 0; i < array->size; i++) {
            markArrayData(gc, array->values[i], (VmDataType) {data->type.type, data->type.array_deph - 1});
        }
        markPtr(gc, array->values);
        markPtr(gc, array);

    }

    markPtr(gc, data);
}

void markQueue(GarbageCollector* gc, Queue* queue) {
    Qitem* curr;
    for (curr = queue->front; curr != NULL; curr = curr->last) {
        if (curr->type == VMOP_DATA) {
            markData(gc, (void*) curr->data);
        }
    }
}

void markVars(GarbageCollector* gc, VmVar** vars, size_t var_num) {
    for (size_t i = 0; i < var_num; i++) {
        if (vars[i]->present) {
            markData(gc, vars[i]->data);
        }
    }
}

void sweep(GarbageCollector* gc) {
    HeapFragment* last = NULL;
    HeapFragment* curr = gc->head;
    while (curr != NULL) {
        if (curr->marked == 0) {
            removePointer(gc, curr, last);

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
