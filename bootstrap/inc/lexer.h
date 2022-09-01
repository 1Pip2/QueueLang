#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

typedef enum TokenType1 {
    _TKTYPE_ID,
    _TKTYPE_KEYWORD,
    _TKTYPE_OP,
    _TKTYPE_SEPARATOR,
    _TKTYPE_LIT,
} TokenType1;
typedef enum TokenType2 {
    TKTYPE_ID,
    
    TKTYPE_FUN,
    TKTYPE_PROG,

    TKTYPE_DO,
    TKTYPE_PLUS,
    TKTYPE_MINUS,
    TKTYPE_ASTERISK,
    TKTYPE_FSLASH,
    TKTYPE_PERCENT,
    TKTYPE_DUMP,

    TKTYPE_COLON,
    TKTYPE_OBRACK,
    TKTYPE_CBRACK,

    TKTYPE_INT,
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
