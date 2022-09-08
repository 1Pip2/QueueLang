#include <stdlib.h>
#include <stdio.h>

#include "vm.h"
#include "gc.h"
#include "queue.h"
#include "errors.h"

void builtinGet(VirtMachine* vm) {
    Qitem* arg1 = dequeue(vm->queue);
    if (arg1->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected type 'data'\n");
        RAISE_TYPE();
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        RAISE_TYPE();
    }

    Qitem* arg2 = dequeue(vm->queue);
    expectQitemDt(vm, arg2, INTDT);

    VmArray* array = (void*) arg1->data->data;
    if (array->size <= arg2->data->data) {
        dumpQueue(vm->queue);
        printf("Error: 'get' out of range\n");
        RAISE_COMMON();
    }

    VmData* data = gcMalloc(vm->gc, sizeof(VmData));
    data->data = array->values[arg2->data->data];
    data->type = (VmDataType) {arg1->data->type.type, arg1->data->type.array_deph - 1};
    enqueue(vm->queue, data, VMOP_DATA);

    free(arg1);
    free(arg2);
}

void builtinAppend(VirtMachine* vm) {
    Qitem* arg1 = dequeue(vm->queue);
    if (arg1->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected type 'data'\n");
        RAISE_TYPE();
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        RAISE_TYPE();
    }

    VmArray* array = (void*) arg1->data->data;
    Qitem* arg2 = dequeue(vm->queue);
    expectQitemDt(vm, arg2, (VmDataType) {arg1->data->type.type, arg1->data->type.array_deph - 1});

    if (array->size == 0) {
        arg1->data->type.type = arg2->data->type.type;
        arg1->data->type.array_deph += arg2->data->type.array_deph;
    }

    appendToArray(vm, array, arg2->data->data);

    free(arg1);
    free(arg2);
}

void builtinPop(VirtMachine* vm) {
    Qitem* arg1 = dequeue(vm->queue);
    if (arg1->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected type 'data'\n");
        RAISE_TYPE();
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        RAISE_TYPE();
    }

    Qitem* arg2 = dequeue(vm->queue);
    expectQitemDt(vm, arg2, INTDT);

    VmArray* array = (void*) arg1->data->data;
    if (array->size <= arg2->data->data) {
        dumpQueue(vm->queue);
        printf("Error: 'pop' out of range\n");
        RAISE_COMMON();
    }

    VmData* data = gcMalloc(vm->gc, sizeof(VmData));
    data->data = array->values[arg2->data->data];
    data->type = (VmDataType) {arg1->data->type.type, arg1->data->type.array_deph - 1};
    enqueue(vm->queue, data, VMOP_DATA);

    for (size_t i = arg2->data->data; i + 1 < array->size; i++) {
        array->values[i] = array->values[i+1];
    }
    array->size--;

    free(arg1);
    free(arg2);
}

void builtinSize(VirtMachine* vm) {
    Qitem* arg1 = dequeue(vm->queue);
    if (arg1->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected type 'data'\n");
        RAISE_TYPE();
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        RAISE_TYPE();
    }

    VmArray* array = (void*) arg1->data->data;
    VmData* data = gcMalloc(vm->gc, sizeof(VmData));
    data->data = array->size;
    data->type = INTDT;
    enqueue(vm->queue, data, VMOP_DATA);

    free(arg1);
}
