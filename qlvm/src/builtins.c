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
        printf("TypeError: Expected arg1 to be of type 'array' in function get\n");
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
}
