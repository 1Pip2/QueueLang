#ifndef VM_H
#define VM_H

#include <stdlib.h>
#include "op.h"
#include "data.h"

typedef enum VmOp {
    VMOP_DATA,
    VMOP_VAR,
    
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

    VMOP_OR,
    VMOP_XOR,
    VMOP_AND,
    VMOP_NOT,

    VMOP_JMPNIF,
    VMOP_JMP,

    VMOP_DUMP,
    VMOP_REQ,
    VMOP_DUP,
    VMOP_CPY,
    VMOP_RM,

    VMOP_LET,
    VMOP_SET,
    VMOP_CALL,

    VMOP_EXIT,
    ARRAYEND,
} VmOp;
typedef enum VmBuiltIns {
    BUILTIN_GET,
    BUILTIN_APPEND,
    BUILTIN_POP,
    BUILTIN_SIZE,
} VmBuiltIns;


typedef struct VmVar {
    VmData* data;
    u_int8_t present;
    u_int8_t writeable;
} VmVar;

struct Queue;
struct GarbageCollector;
typedef struct VirtMachine {
    u_int8_t* ip;
    u_int64_t op_count;
    struct Queue* queue;
    struct GarbageCollector* gc;
    
    VmVar** vars;
    size_t var_num;
} VirtMachine;

typedef struct VmOptions {
    char dumpInfo;
} VmOptions;

VirtMachine* vmInit(u_int8_t*);
_Noreturn void vmInterpret(u_int8_t*, VmOptions*);

#endif
