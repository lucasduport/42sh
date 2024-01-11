#include <stdio.h>
#include <stdlib.h>

#include "src/io_backend/io.h"
#include "src/lexer/lexer.h"
#include "src/utils/string/string.h"

void print_token(struct token token)
{
    printf("Token: %s\n", token.data);

    switch (token.type)
    {
    case TOKEN_IF:
        printf("Type: TOKEN_IF\n");
        break;
    case TOKEN_THEN:
        printf("Type: TOKEN_THEN\n");
        break;
    case TOKEN_ELIF:
        printf("Type: TOKEN_ELIF\n");
        break;
    case TOKEN_ELSE:
        printf("Type: TOKEN_ELSE\n");
        break;
    case TOKEN_FI:
        printf("Type: TOKEN_FI\n");
        break;
    case TOKEN_SEMICOLONS:
        printf("Type: TOKEN_SEMICOLONS\n");
        break;
    case TOKEN_NEWLINE:
        printf("Type: TOKEN_NEWLINE\n");
        break;
    case TOKEN_WORD:
        printf("Type: TOKEN_WORD\n");
        break;
    case TOKEN_EOF:
        printf("Type: TOKEN_EOF\n");
        break;
    default:
        printf("Type: TOKEN_UNKNOWN\n");
        break;
    }
}

int main(int argc, char *argv[])
{
    struct lexer *lexer = lexer_new();

    if (io_abstraction(argc, argv) == IO_FAILED)
        return 1;

    struct token token = lexer_pop(lexer);
    while (token.type != TOKEN_EOF)
    {
        print_token(token);
        free(token.data);

        token = lexer_pop(lexer);
    }

    print_token(token);
    free(token.data);

    lexer_free(lexer);

    return 0;
}
