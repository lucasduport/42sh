#include "executable/execute_ast/execute.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    create_logger("stdout");
    // enable_all_types();

    struct ast *res;
    if (parser(argc, argv, &res) != PARSER_OK)
    {
        debug_printf(LOG_MAIN, "42sh: ast creation failed\n");
        return 2;
    }

    // ast_print(res);

    int code = execute_ast(res);

    ast_free(res);
    destroy_logger();
    return code;
}
