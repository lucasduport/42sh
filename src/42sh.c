#include "executable/execute_ast/execute.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    //create_logger("stdout");
    //enable_log_type(LOG_LEX);
    //enable_log_type(LOG_PARS);
    enable_log_type(LOG_AST);
    enable_log_type(LOG_UTILS);
    enable_log_type(LOG_EXEC);

    // Initialise lexer
    struct lexer *lex = lexer_new(argc, argv);
    if (lex == NULL)
    {
        if (argc > 2 && strlen(argv[2]) == 0)
            return 0;
            
        debug_printf(LOG_MAIN, "[MAIN] Failed initialize lexer\n");
        return 2;
    }

    // Initialise variable used for parsing
    struct ast *res;
    int code = 0;        

    enum parser_status parse_code = parser_input(lex, &res);
    while (parse_code != PARSER_EOF)
    {
        if (parse_code == PARSER_OK)
        {
            if (res != NULL)
            {
                ast_print(res);
                debug_printf(LOG_AST, "\n");
                code = execute_ast(res, NULL);
                ast_free(res);
            }
        }
        else
            code = 2;

        parse_code = parser_input(lex, &res);
    }

    lexer_free(lex);
    destroy_logger();
    return code;
}
