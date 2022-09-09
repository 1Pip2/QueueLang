#ifndef BUILTINS_H
#define BUILTINS_H

struct VmFun;
void builtinGet(struct VmFun*);
void builtinAppend(struct VmFun*);
void builtinPop(struct VmFun*);
void builtinSize(struct VmFun*);

#endif
