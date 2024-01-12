#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    //create_logger("stdout");

    struct ast *res;
    if (parser(argc, argv, &res) != PARSER_OK)
    {
        debug_printf("42sh: ast creation failed\n");
        return 2;
    }

    ast_print(res);
    puts("");

    // TODO: Ast evalutation.

    /*
    create_logger("stdout");

    struct lexer *lexer = lexer_new(argc, argv);

    struct token tok = lexer_pop(lexer);
    while (tok.type != TOKEN_EOF)
    {
        tok = lexer_pop(lexer);

        free(tok.data);
    }
    */

    return 0;
}
