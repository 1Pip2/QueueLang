#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"

typedef enum ByteCode {
    BC_DATA,
    
    BC_DO,
 
    BC_ADD,
    BC_SUB,
    BC_MULT,
    BC_DIV,
    BC_MOD,
    
    BC_EQU,

    BC_DUMP,
    BC_REQ,
    
    BC_EXIT,
} ByteCode;
typedef enum BcDataType {
    DT_INT,
    DT_BOOL,
} BcDataType;
void compileTokens(TkQueue*, char*);

#endif
