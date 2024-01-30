#include "executable/execute_ast/execute.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    create_logger("stdout");
    enable_all_logs();
    disable_all_logs();
    enable_log_type(LOG_PARS);
    disable_log_type(LOG_PARS);

    // Initialise lexer
    struct lexer *lex = lexer_new(argc, argv);
    if (lex == NULL)
    {
        if (argc > 2 && strlen(argv[2]) == 0)
            return 0;

        debug_printf(LOG_MAIN, "[MAIN] Failed initialize lexer\n");
        return 2;
    }

    struct environment *env = environment_new();
    if (env == NULL)
    {
        debug_printf(LOG_MAIN, "[MAIN] Failed initialize env\n");
        return 2;
    }

    set_environment(env, argc, argv);

    // Initialise variable used for parsing
    struct ast *res;
    int code = 0;

    enum parser_status parse_code = PARSER_OK;
    while (parse_code != PARSER_EOF)
    {
        parse_code = parser_input(lex, &res);
        if (parse_code == PARSER_OK || parse_code == PARSER_EOF)
        {
            if (res != NULL)
            {
                code = execute_ast(res, env);
                update_aliases(env);
                ast_free(res);
                if (env->error == STOP)
                    break;
            }
        }
        else
            code = 2;
    }

    lexer_free(lex);
    environment_free(env);
    destroy_logger();
    return code;
}
