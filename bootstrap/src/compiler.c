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
    default:
        printf("Unreachable in compileLit\n");
    }
}

void compileBody(Compiler* compiler) {
    compiler->curr = safePop(compiler);
    expectSecType(compiler->curr, TKTYPE_OBRACK);
    
    while ((compiler->curr = safePop(compiler))->type2 != TKTYPE_CBRACK) {
        if (compiler->curr->type1 == _TKTYPE_OP) {
            appendByte(compiler->outputfile, get_op(compiler->curr->type2));
        } else if (compiler->curr->type1 == _TKTYPE_LIT) {
            compileLit(compiler);
        } else {
            PRINT_DEBUG(compiler->curr);
            printf("SyntaxError\n");
            exit(1);
        }
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
