#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include "vm.h"

#define INTDT (VmDataType) {VMDT_INT, 0}
#define BOOLDT (VmDataType) {VMDT_BOOL, 0}

struct VirtMachine;
struct Qitem;
typedef enum VmBaseType {
    VMDT_INT,
    VMDT_BOOL,
    VMDT_ARRAY,
} VmBaseType;
typedef struct VmDataType {
    VmBaseType type;
    size_t array_deph;
} VmDataType;


typedef struct VmData {
    u_int64_t data;
    VmDataType type;
} VmData;

typedef struct VmArray {
    u_int64_t* values;
    size_t size;
} VmArray;

VmData* initData(struct VirtMachine*);
VmData* copyData(VmDataType, u_int64_t);
void printDataType(VmDataType);
void expectDt(struct VirtMachine*, VmDataType, VmDataType);
void expectQitemDt(struct VirtMachine*, struct Qitem*, VmDataType);
void printData(VmDataType, u_int64_t);

#endif
