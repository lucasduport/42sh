#include "executable/execute_ast/execute.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    // create_logger("stdout");

    struct ast *res;
    if (parser(argc, argv, &res) != PARSER_OK)
    {
        debug_printf("42sh: ast creation failed\n");
        return 2;
    }

    // ast_print(res);

    int code = execute_ast(res);

    ast_free(res);

    return code;
}
