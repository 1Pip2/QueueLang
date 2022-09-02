#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include "vm.h"

struct VirtMachine;
struct Qitem;
typedef enum VmDataType {
    VMDT_INT,
    VMDT_BOOL,
    VMDT_ARRAY,
} VmDataType;

typedef struct VmData {
    u_int64_t data;
    VmDataType type;
} VmData;

typedef struct VmArray {
    VmData** data;
    VmDataType type;
    size_t size;
} VmArray;

VmData* initData(struct VirtMachine*);
VmData* copyData(VmData*);
void printDataType(VmDataType);
void expectDt(struct VirtMachine*, VmDataType, VmDataType);
void expectQitemDt(struct VirtMachine*, struct Qitem*, VmDataType);
void printData(VmData*);
void freeData(VmData*);

#endif
