#ifndef OP_H
#define OP_H

#include <stdlib.h>
#include "vm.h"
#include "data.h"

struct VirtMachine;
struct VmFun;
#define STDOP(op1type, op2type, restype, operation) \
Qitem* op1 = dequeue(fun->queue);                                            \
Qitem* op2 = dequeue(fun->queue);                                            \
\
expectQitemDt(fun, op1, op1type);                                            \
expectQitemDt(fun, op2, op2type);                                            \
\
VmData* new = gcMalloc(fun->gc, sizeof(VmData));                             \
new->type = restype;                                                        \
new->data = (int64_t) op1->data->data operation (int64_t) op2->data->data;  \
enqueue(fun->queue, new, VMOP_DATA);                                         \
\
free(op1);                                                                  \
free(op2);

#define MATHOP(operation) STDOP(INTDT, INTDT, INTDT, operation);
#define COMPOP(operation) STDOP(INTDT, INTDT, BOOLDT, operation);
#define CONDOP(operation) STDOP(BOOLDT, BOOLDT, BOOLDT, operation);

void execAdd(struct VmFun*);
void execSub(struct VmFun*);
void execMult(struct VmFun*);
void execDiv(struct VmFun*);
void execMod(struct VmFun*);

void execEqu(struct VmFun*);
void execGreater(struct VmFun*);
void execLess(struct VmFun*);
void execGreaterEqu(struct VmFun*);
void execLessEqu(struct VmFun*);

void execOr(struct VmFun*);
void execXor(struct VmFun*);
void execAnd(struct VmFun*);
void execNot(struct VmFun*);

void execDump(struct VmFun*);
void execSet(struct VmFun*, u_int8_t, u_int64_t);
void execCall(struct VmFun*, u_int64_t);
void execCallc(struct VirtMachine*, u_int64_t);

#endif
