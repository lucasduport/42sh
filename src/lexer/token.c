#include "token.h"

#include "../logger/logger.h"

struct token token_null(void)
{
    struct token tok;
    tok.type = TOKEN_NULL;
    tok.family = TOKEN_FAM_WORD;
    return tok;
}

void token_free(struct token token)
{
    if (token.data != NULL)
        free(token.data);
}
