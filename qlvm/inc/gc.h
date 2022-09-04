#ifndef GC_H
#define GC_H

#include "vm.h"
#include "queue.h"
#include "data.h"

typedef struct HeapFragment {
    char marked;
    void* ptr;
    struct HeapFragment* next;
} HeapFragment;

typedef struct GarbageCollector {
    HeapFragment* head;
    HeapFragment* tail;
} GarbageCollector;

void gcInit(void);
void* gcMalloc(size_t);
void* gcRealloc(void*, size_t);
void markQueue(Queue*);
void markVars(VmVar**, size_t);
void sweep(void);


#endif
