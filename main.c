#include <stdio.h>
#include <stdlib.h>

#include "src/io_backend/io.h"
#include "src/lexer/lexer.h"
#include "src/utils/string/string.h"

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
