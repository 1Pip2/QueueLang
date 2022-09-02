#ifndef VM_H
#define VM_H

#include <stdlib.h>

typedef enum VmOp {
    VMOP_DATA,
    
    VMOP_DO,
    VMOP_DOALL,

    VMOP_ADD,
    VMOP_SUB,
    VMOP_MULT,
    VMOP_DIV,
    VMOP_MOD,
    
    VMOP_EQU,
    VMOP_GREATER,
    VMOP_LESS,
    VMOP_GREATEREQU,
    VMOP_LESSEQU,

    VMOP_DUMP,
    VMOP_REQ,
    VMOP_DUP,

    VMOP_EXIT,
} VmOp;

typedef enum VmDataType {
    VMDT_INT,
    VMDT_BOOL,
} VmDataType;
typedef struct VmData {
    u_int64_t data;
    VmDataType type;
} VmData;

struct Queue;
typedef struct VirtMachine {
    u_int8_t* ip;
    u_int64_t op_count;
    struct Queue* queue;
} VirtMachine;

typedef struct VmOptions {
    char dumpInfo;
} VmOptions;

VirtMachine* vmInit(u_int8_t*);
_Noreturn void vmInterpret(u_int8_t*, VmOptions*);

#endif
