#include "executable/execute_ast/execute.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    create_logger("stdout");
    enable_log_type(LOG_MAIN);
    enable_log_type(LOG_PARS);
    enable_log_type(LOG_AST);
    enable_log_type(LOG_UTILS);

    //Initialise lexer
    struct lexer *lex = lexer_new(argc, argv);
    if (lex == NULL)
    {
        debug_printf(LOG_MAIN, "[MAIN] Failed initialize lexer\n");
        return 2;
    }

    //Initialise variable used for parsing
    struct ast *res;
    int code = 0;
    enum parser_status parse_code = parser_input(lex, &res);
    while (parse_code != PARSER_EOF_VALID && parse_code != PARSER_EOF_ERROR)
    {
        if (parse_code == PARSER_OK || parse_code == PARSER_EOF_VALID)
        {
            ast_print(res);
            debug_printf(LOG_MAIN, "\n");
            code = execute_ast(res);
            ast_free(res);
        }
        parse_code = parser_input(lex, &res);
    }

    //If end correctly
    if (parse_code == PARSER_EOF_VALID)
    {
        code = execute_ast(res);
        ast_free(res);
    }

    lexer_free(lex);
    //destroy_logger();
    return code;
}
