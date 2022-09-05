#ifndef OP_H
#define OP_H

#include <stdlib.h>
#include "vm.h"
#include "data.h"

struct VirtMachine;
#define STDOP(op1type, op2type, restype, operation) \
Qitem* op1 = dequeue(vm->queue);                                            \
Qitem* op2 = dequeue(vm->queue);                                            \
\
expectQitemDt(vm, op1, op1type);                                            \
expectQitemDt(vm, op2, op2type);                                            \
\
VmData* new = gcMalloc(sizeof(VmData));                                     \
new->type = restype;                                                        \
new->data = (int64_t) op1->data->data operation (int64_t) op2->data->data;  \
enqueue(vm->queue, new, VMOP_DATA);                                         \
\
free(op1);                                                                  \
free(op2);

#define MATHOP(operation) STDOP(INTDT, INTDT, INTDT, operation);
#define COMPOP(operation) STDOP(INTDT, INTDT, BOOLDT, operation);

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
void execSet(struct VirtMachine*, u_int8_t, u_int64_t);
void execExit(struct VirtMachine*);

#endif
