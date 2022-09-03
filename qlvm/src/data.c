#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "errors.h"

VmArray* initArray(VirtMachine* vm) {
    VmArray* array = malloc(sizeof(VmArray));
    array->data = NULL;
    array->size = 0;

    while (*(vm->ip++) != ARRAYEND) {
        array->data = realloc(array->data, (++array->size) * sizeof(VmData*));
        array->data[array->size - 1] = initData(vm);
        if (array->size == 1) {
            array->type = array->data[0]->type;
        } else {
            expectDt(vm, array->data[array->size - 1]->type, array->type);
        }
    }

    return array;
}

VmData* initData(VirtMachine* vm) {
    VmData* new = malloc(sizeof(VmData));
    new->type = *(vm->ip++);

    switch (new->type) {
    case VMDT_INT:
        new->data = *((u_int64_t*) vm->ip);
        vm->ip += 8;
        break;
    case VMDT_BOOL:
        new->data = *(vm->ip++);
        break;
    case VMDT_ARRAY:
        new->data = (u_int64_t) initArray(vm);
        break;
    
    default:
        dumpQueue(vm->queue);
        RAISE_UNREACHABLE();
        break;
    }

    return new;
}

VmData* copyData(VmData* data) {
    VmData* new = malloc(sizeof(VmData));
    if (data->type == VMDT_ARRAY) {
        VmArray* array = malloc(sizeof(VmArray));
        VmArray* old_array = (void*) data->data;

        new->data = (u_int64_t) array;
        array->size = old_array->size;
        array->type = old_array->type;
        array->data = NULL;
        for (size_t i = 0; i < old_array->size; i++) {
            array->data = realloc(array->data, (i + 1) * sizeof(VmData*));
            array->data[i] = copyData(old_array->data[i]);
        }
        
    } else {
        new->data = data->data;
    }

    new->type = data->type;
    return new;
}

void printData(VmData* data) {
     switch (data->type) {
     case VMDT_INT:
        printf("%ld", data->data);
        break;
     case VMDT_BOOL:
        if (data->data) {
            printf("True");
        } else {
            printf("False");
        }
        break;
     case VMDT_ARRAY:
        VmArray* array = (void*) data->data;
        printf("{");
        for (size_t i = 0; i < array->size; i++) {
            printData(array->data[i]);
            if (i < array->size - 1) {
                printf(" ");
            }
        }
        printf("}");
        break;
     
     default:
        RAISE_UNREACHABLE();
        break;
     }
}

void printDataType(VmDataType type) {
     switch (type) {
     case VMDT_INT:
        printf("Int");
        break;
     case VMDT_BOOL:
        printf("Bool");
        break;
     case VMDT_ARRAY:
        printf("Array");
        break;
     
     default:
        RAISE_UNREACHABLE();
        break;
     }
}

void expectDt(VirtMachine* vm, VmDataType type, VmDataType expected) {
    if (type != expected) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected ");
        printDataType(expected);
        printf(". Found ");
        printDataType(type);
        puts("");
        RAISE_COMMON();
    }
}

void expectQitemDt(VirtMachine* vm, Qitem* qitem, VmDataType expected) {
    if (qitem->type != VMOP_DATA) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected type 'data'\n");
        RAISE_TYPE();
    }

    if (qitem->data->type != expected) {
        dumpQueue(vm->queue);
        printf("TypeError: Expected ");
        printDataType(expected);
        printf(". Found ");
        printDataType(qitem->data->type);
        puts("");
        RAISE_TYPE();
    }
}

void freeData(VmData* data) {
    if (data->type == VMDT_ARRAY) {
        VmArray* array = (void*) data->data;
        for (size_t i = 0; i < array->size; i++) {
            freeData(array->data[i]);
        }
    }

    free(data);
}
