#ifndef VM_H
#define VM_H

#include <stdlib.h>

typedef enum VmOp {
    VMOP_DATA,
    
    VMOP_DO,
    
    VMOP_ADD,
    VMOP_SUB,
    VMOP_MULT,
    VMOP_DIV,
    VMOP_MOD,
    
    VMOP_DUMP,
    
    VMOP_EXIT,
} VmOp;

typedef enum VmDataType {
    VMDT_INT,
} VmDataType;
typedef struct VmData {
    u_int64_t data;
    VmDataType type;
} VmData;

struct Queue;
typedef struct VirtMachine {
    u_int8_t* ip;
    struct Queue* queue;
} VirtMachine;

VirtMachine* vmInit(u_int8_t*);
_Noreturn void vmInterpret(u_int8_t*);

#endif
