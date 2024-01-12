#include "lexer/lexer.h"
#include "parser/parser.h"
#include "executable/execute_ast/execute.h"

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

    execute_ast(res);

    /*
    create_logger("stdout");

    debug_printf("LEXER_PEEK");
    struct lexer *lexer = lexer_new(argc, argv);

    struct token tok = lexer_peek(lexer);
    while (tok.type != TOKEN_EOF)
    {
        debug_printf("offset after peek: %zu\n", lexer->offset);
        
        debug_printf("LEXER_POP");
        tok = lexer_pop(lexer);
        debug_printf("offset after pop: %zu\n", lexer->offset);

        debug_printf("LEXER_PEEK");
        tok = lexer_peek(lexer);

        free(tok.data);
    }
    */

    return 0;
}
