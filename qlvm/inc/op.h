#ifndef OP_H
#define OP_H

#include <stdlib.h>
#include "vm.h"
#include "data.h"

struct VirtMachine;
#define STDOP(op1type, op2type, restype, operation) \
Qitem* op1 = dequeue(vm->queue); \
Qitem* op2 = dequeue(vm->queue); \
\
expectQitemDt(vm, op1, op1type); \
expectQitemDt(vm, op2, op2type); \
\
VmData* new = malloc(sizeof(VmData)); \
new->type = restype; \
new->data = op1->data->data operation op2->data->data; \
enqueue(vm->queue, new, VMOP_DATA); \
\
freeData(op1->data); \
freeData(op2->data); \
free(op1); \
free(op2);

#define MATHOP(operation) STDOP(VMDT_INT, VMDT_INT, VMDT_INT, operation);
#define COMPOP(operation) STDOP(VMDT_INT, VMDT_INT, VMDT_BOOL, operation);

void execAdd(struct VirtMachine*);
void execSub(struct VirtMachine*);
void execMult(struct VirtMachine*);
void execDiv(struct VirtMachine*);
void execMod(struct VirtMachine*);

void execEqu(struct VirtMachine*);
void execGreater(struct VirtMachine*);
void execLess(struct VirtMachine*);
void execGreaterEqu(struct VirtMachine*);
void execLessEqu(struct VirtMachine*);

void execDump(struct VirtMachine*);
void execExit(struct VirtMachine*);

#endif
