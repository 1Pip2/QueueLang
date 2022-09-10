#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

Token* safeNext(Token* token) {
    if (token->last == NULL) {
        PRINT_DEBUG(token);
        puts("Error: Expected Token; Found EOF");
        exit(1);
    }
    return token->last;
}

void preprocessTokens(TkQueue* tokens) {
    Token* curr;
    Token* tmp = NULL;
    for (curr = tokens->front; curr != NULL; curr = curr->last) {
        if (curr->type2 == TKTYPE_IMPORT) {
            if (safeNext(curr)->type2 != TKTYPE_STR) {
                PRINT_DEBUG(curr->last);
                puts("Error: Expect \"<path>\" after import");
                exit(1);
            }

            TkQueue* file_tokens = lexFile(curr->last->stringdata);

            tokens->back->last = file_tokens->front;
            tokens->back = file_tokens->back;
            if (tmp == NULL) {
                tokens->front = curr->last->last;
            } else {
                tmp->last = curr->last->last;
            }
            curr = curr->last;
        } else {
            tmp = curr;
        }
    }
}
