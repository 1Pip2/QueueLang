#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "errors.h"

VmArray* initArray(VirtMachine* vm, VmData* data) {
    VmArray* array = malloc(sizeof(VmArray));
    array->values = NULL;
    array->size = 0;

    VmData* curr;
    while (*(vm->ip++) != ARRAYEND) {
        array->values = realloc(array->values, (++array->size) * sizeof(u_int64_t));
        curr = initData(vm);

        array->values[array->size - 1] = curr->data;

        if (array->size == 1) {
            data->type.array_deph += curr->type.array_deph;
            data->type.type = curr->type.type;
        } else {
            expectDt(vm, curr->type, (VmDataType) {data->type.type, data->type.array_deph - 1});
        }
        free(curr);
    }

    return array;
}

VmData* initData(VirtMachine* vm) {
    VmData* new = malloc(sizeof(VmData));
    VmBaseType base_type = *(vm->ip++);

    new->type.array_deph = 0;
    new->type.type = base_type;
    switch (base_type) {
    case VMDT_INT:
        new->data = *((u_int64_t*) vm->ip);
        vm->ip += 8;
        break;
    case VMDT_BOOL:
        new->data = *(vm->ip++);
        break;
    case VMDT_ARRAY:
        new->type.array_deph++;
        new->data = (u_int64_t) initArray(vm, new);
        break;
    
    default:
        dumpQueue(vm->queue);
        RAISE_UNREACHABLE();
        break;
    }

    return new;
}

VmData* copyData(VmDataType type, u_int64_t data) {
    VmData* new = malloc(sizeof(VmData));
    if (type.array_deph > 0) {
        VmArray* array = malloc(sizeof(VmArray));
        VmArray* old_array = (void*) data;

        new->data = (u_int64_t) array;
        array->size = old_array->size;
        array->values = NULL;

        VmDataType elementtype = type;
        elementtype.array_deph--;
        VmData* curr;
        for (size_t i = 0; i < old_array->size; i++) {
            curr = copyData(elementtype, old_array->values[i]);
            array->values = realloc(array->values, (i + 1) * sizeof(u_int64_t));
            array->values[i] = curr->data;
            free(curr);
        }
        
    } else {
        new->data = data;
    }

    new->type = type;
    return new;
}

void printData(VmDataType type, u_int64_t data) {
    if (type.array_deph > 0) {
        VmArray* array = (void*) data;
        printf("{");

        VmDataType elementtype = type;
        elementtype.array_deph--;
        for (size_t i = 0; i < array->size; i++) {
            printData(elementtype, array->values[i]);
            if (i < array->size - 1) {
                printf(" ");
            }
        }
        printf("}");
        return;
    }

    switch (type.type) {
    case VMDT_INT:
        printf("%ld", data);
        break;
    case VMDT_BOOL:
        if (data) {
            printf("True");
        } else {
            printf("False");
        }
        break;
    
    default:
        RAISE_UNREACHABLE();
        break;
    }
}

void printDataType(VmDataType type) {
    if (type.array_deph > 0) {
        printf("[");
        VmDataType elementtype = type;
        elementtype.array_deph--;
        printDataType(elementtype);
        printf("]");
        return;
    }

     switch (type.type) {
     case VMDT_INT:
        printf("Int");
        break;
     case VMDT_BOOL:
        printf("Bool");
        break;
     
     default:
        RAISE_UNREACHABLE();
        break;
     }
}

void expectDt(VirtMachine* vm, VmDataType type, VmDataType expected) {
    if (type.type != expected.type || type.array_deph != expected.array_deph) {
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

    expectDt(vm, qitem->data->type, expected);
}
