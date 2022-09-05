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

GarbageCollector* gcInit(void);
void* gcMalloc(GarbageCollector*, size_t);
void* gcRealloc(GarbageCollector*, void*, size_t);
void markQueue(GarbageCollector*, Queue*);
void markVars(GarbageCollector*, VmVar**, size_t);
void sweep(GarbageCollector*);


#endif
