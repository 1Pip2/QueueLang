#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"

typedef enum ByteCode {
    BC_DATA,
    
    BC_DO,
    BC_DOALL,

    BC_ADD,
    BC_SUB,
    BC_MULT,
    BC_DIV,
    BC_MOD,

    BC_EQU,
    BC_GREATER,
    BC_LESS,
    BC_GREATEREQU,
    BC_LESSEQU,

    BC_DUMP,
    BC_REQ,
    BC_DUP,
    
    BC_EXIT,
    ENDARRAY,
} ByteCode;
typedef enum BcDataType {
    DT_INT,
    DT_BOOL,
    DT_ARRAY,
} BcDataType;
void compileTokens(TkQueue*, char*);

#endif
