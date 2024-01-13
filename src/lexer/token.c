#include "token.h"

#include "../logger/logger.h"

void print_token(struct token token)
{
    debug_printf("[TOKEN] Data: %s\n", token.data);
    switch (token.type)
    {
    case TOKEN_IF:
        debug_printf("[TOKEN] Type: TOKEN_IF\n");
        break;
    case TOKEN_THEN:
        debug_printf("[TOKEN] Type: TOKEN_THEN\n");
        break;
    case TOKEN_ELIF:
        debug_printf("[TOKEN] Type: TOKEN_ELIF\n");
        break;
    case TOKEN_ELSE:
        debug_printf("[TOKEN] Type: TOKEN_ELSE\n");
        break;
    case TOKEN_FI:
        debug_printf("[TOKEN] Type: TOKEN_FI\n");
        break;
    case TOKEN_SEMICOLONS:
        debug_printf("[TOKEN] Type: TOKEN_SEMICOLONS\n");
        break;
    case TOKEN_NEWLINE:
        debug_printf("[TOKEN] Type: TOKEN_NEWLINE\n");
        break;
    case TOKEN_WORD:
        debug_printf("[TOKEN] Type: TOKEN_WORD\n");
        break;
    case TOKEN_EOF:
        debug_printf("[TOKEN] Type: TOKEN_EOF\n");
        break;
    default:
        debug_printf("[TOKEN] Type: TOKEN_UNKNOWN\n");
        break;
    }
}

void token_free(struct token token)
{
    free(token.data);
}
