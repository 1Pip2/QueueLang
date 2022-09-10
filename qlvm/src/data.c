#include <stdio.h>
#include <stdlib.h>

#include "gc.h"
#include "data.h"
#include "errors.h"

void appendToArray(VmFun* fun, VmArray* array, u_int64_t data) {
    if (array->size >= array->totalsize) {
        if (array->totalsize == 0) {
            array->totalsize = 1;
        } else {
            array->totalsize *= 2;
        }
    }

    array->values = gcRealloc(array->values, (array->totalsize) * sizeof(u_int64_t));
    array->values[array->size++] = data;
}

VmArray* initArray(VmFun* fun, VmData* data) {
    VmArray* array = gcMalloc(sizeof(VmArray));
    array->values = NULL;
    array->size = 0;
    array->totalsize = 0;

    VmData* curr;
    while (*(fun->ip++) != ARRAYEND) {
        curr = initData(fun);
        appendToArray(fun, array, curr->data);

        if (array->size == 1) {
            data->type.array_deph += curr->type.array_deph;
            data->type.type = curr->type.type;
        } else {
            expectDt(fun, curr->type, (VmDataType) {data->type.type, data->type.array_deph - 1});
        }
    }

    if (array->size == 0) {
        data->type.type = VMDT_UNKNOWN;
    }

    return array;
}

VmData* initData(VmFun* fun) {
    VmData* new = gcMalloc(sizeof(VmData));
    VmBaseType base_type = *(fun->ip++);

    new->type.array_deph = 0;
    new->type.type = base_type;
    switch (base_type) {
    case VMDT_INT:
        new->data = *((u_int64_t*) fun->ip);
        fun->ip += 8;
        break;
    case VMDT_BOOL:
        new->data = *(fun->ip++);
        break;
    case VMDT_ARRAY:
        new->type.array_deph++;
        new->data = (u_int64_t) initArray(fun, new);
        break;
    
    default:
        RAISE_UNREACHABLE(fun->queue);
    }

    return new;
}

VmData* copyData(VmFun* fun, VmDataType type, u_int64_t data) {
    VmData* new = gcMalloc(sizeof(VmData));
    if (type.array_deph > 0) {
        VmArray* array = gcMalloc(sizeof(VmArray));
        VmArray* old_array = (void*) data;

        new->data = (u_int64_t) array;
        array->size = old_array->size;
        array->values = NULL;

        VmDataType elementtype = type;
        elementtype.array_deph--;
        VmData* curr;
        for (size_t i = 0; i < old_array->size; i++) {
            curr = copyData(fun, elementtype, old_array->values[i]);
            array->values = gcRealloc(array->values, (i + 2) * sizeof(u_int64_t));
            array->values[i] = curr->data;
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
    case VMDT_UNKNOWN:
        break;
    
    default:
        RAISE_RAW_UNREACHABLE();
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
    case VMDT_UNKNOWN:
        printf("?");
        break;
     
     default:
        RAISE_RAW_UNREACHABLE();
     }
}

void expectDt(VmFun* fun, VmDataType type, VmDataType expected) {
    if (type.type == VMDT_UNKNOWN) {
        return;
    }

    if ((type.type != expected.type && expected.type != VMDT_UNKNOWN) || type.array_deph != expected.array_deph) {
        dumpQueue(fun->queue);
        printf("TypeError: Expected ");
        printDataType(expected);
        printf(". Found ");
        printDataType(type);
        puts("");
        exit(ERR_TYPE);
    }
}

void expectQitemDt(VmFun* fun, Qitem* qitem, VmDataType expected) {
    if (qitem->type != VMOP_DATA) {
        RAISE_EXPECT_DATA(fun->queue);
    }

    expectDt(fun, qitem->data->type, expected);
}
