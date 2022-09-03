#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "compiler.h"

typedef struct Compiler {
    Token* curr;
    TkQueue* tokens;
    char* outputfile;

    char progdef;
    char endif;
    u_int64_t* ifrets;
    size_t ifret_num;
} Compiler;

void eofError(Token* token) {
    PRINT_DEBUG(token);
    printf("Error: Expectd Token. Found EOF\n");
    exit(1);
}

Token* safePop(Compiler* compiler) {
    Token* token = tkQueuePop(compiler->tokens);
    if (token == NULL) {
        eofError(compiler->curr);
    }

    return token;
}

long getFileLen(char* filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error: Unable to open '%s'\n", filename);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fclose(fp);
    return len;
}

void appendByte(char* filename, u_int8_t byte) {
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        printf("Error: Unable to create or edit '%s'\n", filename);
        exit(1);
    }

    fprintf(fp, "%c", byte);
    fclose(fp);
}

void appendQuad(char* filename, u_int64_t data) {
    for (size_t i = 0; i < 8; i++) {
        appendByte(filename, (data >> i * 8) & 0xFF);
    }
}

void setByte(char* filename, u_int8_t byte, size_t pos) {
    FILE *fp = fopen(filename, "r+");
    if (fp == NULL) {
        printf("Error: Unable to create or edit '%s'\n", filename);
        exit(1);
    }

    fseek(fp, pos, SEEK_SET);
    fprintf(fp, "%c", byte);
    fclose(fp);
}

void setQuad(char* filename, u_int64_t data, size_t pos) {
    for (size_t i = 0; i < 8; i++) {
        setByte(filename, (data >> i * 8) & 0xFF, pos + i);
    }
}

char* getOutputFileName(char* filename) {
    size_t filename_len = strlen(filename);
    char* outfile = malloc((filename_len + 4) * sizeof(char));

    strcpy(outfile, filename);
    outfile[filename_len + 0] = '.';
    outfile[filename_len + 1] = 'b';
    outfile[filename_len + 2] = 'c';
    outfile[filename_len + 3] = '\0';

    return outfile;
}

void expectPrimeType(Token* token, TokenType1 type) {
    if (token->type1 != type) {
        PRINT_DEBUG(token);
        printf("SyntaxError\n");
        exit(1);
    }
}
void expectSecType(Token* token, TokenType2 type) {
    if (token->type2 != type) {
        PRINT_DEBUG(token);
        printf("SyntaxError\n");
        exit(1);
    }
}

ByteCode get_op(TokenType2 type) {
    switch (type) {
    case TKTYPE_DO:
        return BC_DO;
    case TKTYPE_DOALL:
        return BC_DOALL;

    case TKTYPE_PLUS:
        return BC_ADD;
    case TKTYPE_MINUS:
        return BC_SUB;
    case TKTYPE_ASTERISK:
        return BC_MULT;
    case TKTYPE_FSLASH:
        return BC_DIV;
    case TKTYPE_PERCENT:
        return BC_MOD;

    case TKTYPE_EQUALS:
        return BC_EQU;
    case TKTYPE_GREATER:
        return BC_GREATER;
    case TKTYPE_LESS:
        return BC_LESS;
    case TKTYPE_GREATEREQU:
        return BC_GREATEREQU;
    case TKTYPE_LESSEQU:
        return BC_LESSEQU;
    
    case TKTYPE_DUMP:
        return BC_DUMP;
    case TKTYPE_REQ:
        return BC_REQ;
    case TKTYPE_DUP:
        return BC_DUP;
    
    default:
        printf("Unreachable in get_op\n");
        exit(1);
    }
}

void compileBody(Compiler*);
void compileArrayLit(Compiler*);
void compileLit(Compiler* compiler) {
    switch (compiler->curr->type2) {
    case TKTYPE_INT:
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_INT);
        appendQuad(compiler->outputfile, compiler->curr->numdata);
        break;
    case TKTYPE_TRUE:
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_BOOL);
        appendByte(compiler->outputfile, 1);
        break;
    case TKTYPE_FALSE:
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_BOOL);
        appendByte(compiler->outputfile, 0);
        break;
    case TKTYPE_OBRACE:
        compileArrayLit(compiler);
        break;
    default:
        printf("Unreachable in compileLit\n");
    }
}

void compileIfStatement(Compiler* compiler) {
    if (compiler->endif) {  // if this is an elif statement
        compiler->endif = 0;  // prevent error in recursive compileBody
        compiler->ifrets = realloc(compiler->ifrets, (++compiler->ifret_num) * sizeof(u_int64_t));
    } else {
        compiler->ifrets = malloc(sizeof(u_int64_t));
        compiler->ifret_num = 1;
    }

    appendByte(compiler->outputfile, BC_JMPNIF);
    size_t ifpos = getFileLen(compiler->outputfile);
    appendQuad(compiler->outputfile, 0);
    
    compileBody(compiler);

    appendByte(compiler->outputfile, BC_JMP);
    size_t ifret = getFileLen(compiler->outputfile);
    appendQuad(compiler->outputfile, 0);
    
    setQuad(compiler->outputfile, ifret + 8, ifpos);  // jump to else
    compiler->endif = 1;
    compiler->ifrets[compiler->ifret_num - 1] = ifret;
}

void compileEndIfStatement(Compiler* compiler) {
    if (compiler->endif == 0) {
        PRINT_DEBUG(compiler->curr);
        printf("SyntaxError: endif without if");
        exit(1);
    }

    for (size_t i = 0; i < compiler->ifret_num; i++) {
        setQuad(compiler->outputfile, getFileLen(compiler->outputfile), compiler->ifrets[i]);
    }    
    compiler->endif = 0;
}

void compileArrayLit(Compiler* compiler) {
        appendByte(compiler->outputfile, BC_DATA);
        appendByte(compiler->outputfile, DT_ARRAY);

        while ((compiler->curr = safePop(compiler))->type2 != TKTYPE_CBRACE) {
            compileLit(compiler);
        }
        appendByte(compiler->outputfile, ENDARRAY);
}

void compileBody(Compiler* compiler) {
    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_OBRACK);
    
    while ((compiler->curr = safePop(compiler))->type2 != TKTYPE_CBRACK) {
        if (compiler->curr->type1 == _TKTYPE_OP) {
            appendByte(compiler->outputfile, get_op(compiler->curr->type2));
        } else if (compiler->curr->type1 == _TKTYPE_LIT) {
            compileLit(compiler);
        } else if (compiler->curr->type2 == TKTYPE_OBRACE) {
            compileArrayLit(compiler);
        } else if (compiler->curr->type2 == TKTYPE_IF) {
            compileIfStatement(compiler);
        } else if (compiler->curr->type2 == TKTYPE_ENDIF) {
            compileEndIfStatement(compiler);
        } else {
            PRINT_DEBUG(compiler->curr);
            printf("SyntaxError\n");
            exit(1);
        }
    }

    if (compiler->endif) {
        PRINT_DEBUG(compiler->curr);
        printf("SyntaxError: Expected endif\n");
        exit(1);
    }
}

void compileProg(Compiler* compiler) {
    if (compiler->progdef) {
        PRINT_DEBUG(compiler->curr);
        printf("Error: Prog already defined\n");
        exit(1);
    }
    compiler->progdef = 1;

    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_COLON);

    compileBody(compiler);

    appendByte(compiler->outputfile, BC_EXIT);

    appendByte(compiler->outputfile, BC_DATA);
    appendByte(compiler->outputfile, DT_INT);
    appendQuad(compiler->outputfile, 0);
    
    appendByte(compiler->outputfile, BC_DO);
}

void compileTokens(TkQueue* tokens, char* filename) {
    Compiler* compiler = malloc(sizeof(Compiler));
    char* outfile = getOutputFileName(filename);
    remove(outfile);

    compiler->outputfile = outfile;
    compiler->tokens = tokens;
    compiler->progdef = 0;

    while ((compiler->curr = tkQueuePop(compiler->tokens)) != NULL) {
        switch (compiler->curr->type2) {
        case TKTYPE_PROG:
            compileProg(compiler);
            break;
        
        default:
            PRINT_DEBUG(compiler->curr);
            printf("SyntaxError\n");
            exit(1);
        }
    }
}
