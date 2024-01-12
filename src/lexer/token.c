#include "token.h"

#include "../logger/logger.h"

void print_token(struct token token)
{
    debug_printf("[TOKEN] Data: %s", token.data);
    switch (token.type)
    {
    case TOKEN_IF:
        debug_printf("[TOKEN] Type: TOKEN_IF");
        break;
    case TOKEN_THEN:
        debug_printf("[TOKEN] Type: TOKEN_THEN");
        break;
    case TOKEN_ELIF:
        debug_printf("[TOKEN] Type: TOKEN_ELIF");
        break;
    case TOKEN_ELSE:
        debug_printf("[TOKEN] Type: TOKEN_ELSE");
        break;
    case TOKEN_FI:
        debug_printf("[TOKEN] Type: TOKEN_FI");
        break;
    case TOKEN_SEMICOLONS:
        debug_printf("[TOKEN] Type: TOKEN_SEMICOLONS");
        break;
    case TOKEN_NEWLINE:
        debug_printf("[TOKEN] Type: TOKEN_NEWLINE");
        break;
    case TOKEN_WORD:
        debug_printf("[TOKEN] Type: TOKEN_WORD");
        break;
    case TOKEN_EOF:
        debug_printf("[TOKEN] Type: TOKEN_EOF");
        break;
    default:
        debug_printf("[TOKEN] Type: TOKEN_UNKNOWN");
        break;
    }
}

void token_free(struct token token)
{
    free(token.data);
}
