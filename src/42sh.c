#include "lexer/lexer.h"
#include "parser/parser.h"
#include "executable/execute_ast/execute.h"

int main(int argc, char **argv)
{
    create_logger("stdout");

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

    struct lexer *lexer = lexer_new(argc, argv);

    //debug_printf("PEEK");
    struct token tok = lexer_pop(lexer);
    while (tok.type != TOKEN_EOF)
    {
        //debug_printf("POP");
        tok = lexer_pop(lexer);
        //debug_printf("PEEK");
        //tok = lexer_peek(lexer);

        token_free(tok);
    }
    //debug_printf("POP");
    //tok = lexer_pop(lexer);
    */

    return 0;
}
