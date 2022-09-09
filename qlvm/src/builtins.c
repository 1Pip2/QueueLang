#include <stdlib.h>
#include <stdio.h>

#include "vm.h"
#include "gc.h"
#include "queue.h"
#include "errors.h"

void builtinGet(VmFun* fun) {
    Qitem* arg1 = dequeue(fun->queue);
    if (arg1->type != VMOP_DATA) {
        RAISE_EXPECT_DATA(fun->queue);
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(fun->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        exit(ERR_TYPE);
    }

    Qitem* arg2 = dequeue(fun->queue);
    expectQitemDt(fun, arg2, INTDT);

    VmArray* array = (void*) arg1->data->data;
    if (array->size <= arg2->data->data) {
        dumpQueue(fun->queue);
        printf("Error: 'get' out of range\n");
        exit(ERR_COMMON);
    }

    VmData* data = gcMalloc(fun->gc, sizeof(VmData));
    data->data = array->values[arg2->data->data];
    data->type = (VmDataType) {arg1->data->type.type, arg1->data->type.array_deph - 1};
    enqueue(fun->queue, data, VMOP_DATA);

    free(arg1);
    free(arg2);
}

void builtinAppend(VmFun* fun) {
    Qitem* arg1 = dequeue(fun->queue);
    if (arg1->type != VMOP_DATA) {
        RAISE_EXPECT_DATA(fun->queue);
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(fun->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        exit(ERR_TYPE);
    }

    VmArray* array = (void*) arg1->data->data;
    Qitem* arg2 = dequeue(fun->queue);
    expectQitemDt(fun, arg2, (VmDataType) {arg1->data->type.type, arg1->data->type.array_deph - 1});

    if (array->size == 0) {
        arg1->data->type.type = arg2->data->type.type;
        arg1->data->type.array_deph += arg2->data->type.array_deph;
    }

    appendToArray(fun, array, arg2->data->data);

    free(arg1);
    free(arg2);
}

void builtinPop(VmFun* fun) {
    Qitem* arg1 = dequeue(fun->queue);
    if (arg1->type != VMOP_DATA) {
        RAISE_EXPECT_DATA(fun->queue);
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(fun->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        exit(ERR_TYPE);
    }

    Qitem* arg2 = dequeue(fun->queue);
    expectQitemDt(fun, arg2, INTDT);

    VmArray* array = (void*) arg1->data->data;
    if (array->size <= arg2->data->data) {
        dumpQueue(fun->queue);
        printf("Error: 'pop' out of range\n");
        exit(ERR_COMMON);
    }

    VmData* data = gcMalloc(fun->gc, sizeof(VmData));
    data->data = array->values[arg2->data->data];
    data->type = (VmDataType) {arg1->data->type.type, arg1->data->type.array_deph - 1};
    enqueue(fun->queue, data, VMOP_DATA);

    for (size_t i = arg2->data->data; i + 1 < array->size; i++) {
        array->values[i] = array->values[i+1];
    }
    array->size--;

    free(arg1);
    free(arg2);
}

void builtinSize(VmFun* fun) {
    Qitem* arg1 = dequeue(fun->queue);
    if (arg1->type != VMOP_DATA) {
        RAISE_EXPECT_DATA(fun->queue);
    }
    if (arg1->data->type.array_deph == 0) {
        dumpQueue(fun->queue);
        printf("TypeError: Expected arg1 to be of type 'array'\n");
        exit(ERR_TYPE);
    }

    VmArray* array = (void*) arg1->data->data;
    VmData* data = gcMalloc(fun->gc, sizeof(VmData));
    data->data = array->size;
    data->type = INTDT;
    enqueue(fun->queue, data, VMOP_DATA);

    free(arg1);
}
