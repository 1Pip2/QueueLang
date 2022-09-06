#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"

typedef enum ByteCode {
    BC_DATA,
    BC_VAR,
    
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

    BC_OR,
    BC_XOR,
    BC_AND,
    BC_NOT,

    BC_JMPNIF,
    BC_JMP,

    BC_DUMP,
    BC_REQ,
    BC_DUP,
    BC_CPY,
    BC_RM,

    BC_LET,
    BC_SET,
    BC_CALL,
    
    BC_EXIT,
    ENDARRAY,
} ByteCode;
typedef enum BcDataType {
    DT_INT,
    DT_BOOL,
    DT_ARRAY,
} BcDataType;
typedef enum BcBuiltIns {
    BUILTIN_GET,
    BUILTIN_APPEND,
} BcBuiltIns;

void compileTokens(TkQueue*, char*);

#endif
