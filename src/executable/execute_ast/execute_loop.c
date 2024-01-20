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

int execute_for(struct ast *ast, struct environment *env)
{
    char *for_var = ast->arg->current;
    if (for_var == NULL)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Missing condition\n");
        return -1;
    }

    int ret_code = 0;
    struct list *for_cond_exp = expansion(ast->arg->next, env, &ret_code);
    if (for_cond_exp == NULL)
        return ret_code;

    for (struct list *temp = for_cond_exp; temp != NULL; temp = temp->next)
    {
        if (set_variable(&(env->variables), for_var, temp->current) == -1)
        {
            debug_printf(LOG_EXEC, "[EXECUTE] Variables assignment failed\n");
            list_destroy(for_cond_exp);
            return 2;
        }

        ret_code = execute_ast(ast->first_child, env);
    }
    list_destroy(for_cond_exp);
    return ret_code;
}
