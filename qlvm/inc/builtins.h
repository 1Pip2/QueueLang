#ifndef BUILTINS_H
#define BUILTINS_H

struct VirtMachine;
void builtinGet(struct VirtMachine*);
void builtinAppend(struct VirtMachine*);
void builtinPop(struct VirtMachine*);
void builtinSize(struct VirtMachine*);

#endif
