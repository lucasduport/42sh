#include "execute.h"

int execute_while(struct ast *ast, struct environment *env)
{
    ast = ast->first_child;
    debug_printf(LOG_EXEC, "[EXECUTE] In execute while\n");
    if (ast == NULL || ast->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or do for 'while' node\n");
        return -1;
    }

    int ret_code = 0;
    while (execute_ast(ast, env) == 0)
    {
        debug_printf(LOG_EXEC, "[EXEC] In while loop\n");
        ret_code = execute_ast(ast->next, env);
    }

    return ret_code;
}

int execute_until(struct ast *ast, struct environment *env)
{
    ast = ast->first_child;
    debug_printf(LOG_EXEC, "[EXECUTE] In execute until\n");
    if (ast == NULL || ast->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or do for 'until' node\n");
        return -1;
    }

    int ret_code = 0;
    while (execute_ast(ast, env) != 0)
        ret_code = execute_ast(ast->next, env);

    return ret_code;
}
