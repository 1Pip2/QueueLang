#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

typedef enum TokenType1 {
    _TKTYPE_ID,
    _TKTYPE_KEYWORD,
    _TKTYPE_OP,
    _TKTYPE_SEPARATOR,
    _TKTYPE_LIT,

    _TKTYPE_PREPROCESS,
} TokenType1;
typedef enum TokenType2 {
    TKTYPE_ID,
    
    TKTYPE_FUN,
    TKTYPE_IF,
    TKTYPE_ENDIF,
    TKTYPE_WHILE,
    TKTYPE_PROG,

    TKTYPE_DO,
    TKTYPE_DOALL,

    TKTYPE_PLUS,
    TKTYPE_MINUS,
    TKTYPE_ASTERISK,
    TKTYPE_FSLASH,
    TKTYPE_PERCENT,

    TKTYPE_EQUALS,
    TKTYPE_GREATER,
    TKTYPE_LESS,
    TKTYPE_LESSEQU,
    TKTYPE_GREATEREQU,
    
    TKTYPE_OR,
    TKTYPE_XOR,
    TKTYPE_AND,
    TKTYPE_NOT,
    
    TKTYPE_DUMP,
    TKTYPE_REQ,
    TKTYPE_DUP,
    TKTYPE_CPY,
    TKTYPE_RM,

    TKTYPE_LET,
    TKTYPE_SET,
    TKTYPE_CALL,
    TKTYPE_RET,

    TKTYPE_COLON,
    TKTYPE_OBRACK,
    TKTYPE_CBRACK,
    TKTYPE_OBRACE,
    TKTYPE_CBRACE,
    TKTYPE_OPAREN,
    TKTYPE_CPAREN,

    TKTYPE_STR,
    TKTYPE_INT,
    TKTYPE_TRUE,
    TKTYPE_FALSE,

    TKTYPE_IMPORT,
} TokenType2;

typedef struct Token {
    TokenType1 type1;
    TokenType2 type2;

    char* stringdata;
    u_int64_t numdata;

    char* filename;
    size_t line;

    struct Token* last;
} Token;

typedef struct TkQueue {
    Token* front;
    Token* back;
} TkQueue;

TkQueue* lexFile(char*);
Token* tkQueuePop(TkQueue*);

#define PRINT_DEBUG(token) printf("In %s::%lu\n", token->filename, token->line);

#endif
