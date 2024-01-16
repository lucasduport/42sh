#include "token.h"

#include "../logger/logger.h"

void print_token(struct token token)
{
    debug_printf(LOG_LEX,"[TOKEN] Data: %s\n", token.data);
    switch (token.type)
    {
    case TOKEN_IF:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_IF\n");
        break;
    case TOKEN_THEN:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_THEN\n");
        break;
    case TOKEN_ELIF:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_ELIF\n");
        break;
    case TOKEN_ELSE:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_ELSE\n");
        break;
    case TOKEN_FI:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_FI\n");
        break;
    case TOKEN_SEMICOLONS:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_SEMICOLONS\n");
        break;
    case TOKEN_NEWLINE:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_NEWLINE\n");
        break;
    case TOKEN_WORD:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_WORD\n");
        break;
    case TOKEN_EOF:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_EOF\n");
        break;
    default:
        debug_printf(LOG_LEX,"[TOKEN] Type: TOKEN_UNKNOWN\n");
        break;
    }
}

struct token token_null()
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
