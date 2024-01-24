#include "execute.h"

/**
 * @brief Check if we need to break or continue in a loop
 * 
 * @param env Current environment
 * 
 * @return 
 *  0 -> nothing or continue
 *  1 -> break
*/
static int check_break_continue(struct environment *env)
{
    if (env->nb_break > 0)
    {
        env->nb_break -= 1;
        env->nb_loop -= 1;
        return 1;
    }
        
    if (env->nb_continue == 1 || env->nb_loop == 1)
    {
        env->nb_continue = 0;
        return 0;
    }
    else if (env->nb_continue > 1)
    {
        env->nb_continue -= 1;
        env->nb_loop -= 1;
        return 1;
    }
    return 0;
}

int execute_while(struct ast *ast, struct environment *env)
{
    env->nb_loop++;
    ast = ast->first_child;
    debug_printf(LOG_EXEC, "[EXECUTE] In execute while\n");
    if (ast == NULL || ast->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or do for 'while' node\n");
        return -1;
    }

    int ret_code = 0;
    while (execute_ast(ast, env) == 0 && !env->exit)
    {
        debug_printf(LOG_EXEC, "[EXEC] In while loop\n");
        ret_code = execute_ast(ast->next, env);

        // Check break and continue
        if (check_break_continue(env) == 1)
            break;
    }

    return ret_code;
}

int execute_until(struct ast *ast, struct environment *env)
{
    env->nb_loop++;
    ast = ast->first_child;
    debug_printf(LOG_EXEC, "[EXECUTE] In execute until\n");
    if (ast == NULL || ast->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or do for 'until' node\n");
        return -1;
    }

    int ret_code = 0;
    while (execute_ast(ast, env) != 0 && !env->exit)
    {
        ret_code = execute_ast(ast->next, env);
        if (check_break_continue(env) == 1)
            break;
    }

    return ret_code;
}

int execute_for(struct ast *ast, struct environment *env)
{
    env->nb_loop++;
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

    for (struct list *temp = for_cond_exp; temp != NULL && !env->exit;
         temp = temp->next)
    {
        if (set_variable(env, for_var, temp->current) == -1)
        {
            debug_printf(LOG_EXEC, "[EXECUTE] Variables assignment failed\n");
            list_destroy(for_cond_exp);
            return 2;
        }

        ret_code = execute_ast(ast->first_child, env);

        // Check break and continue
        if (check_break_continue(env) == 1)
            break;
        
    }
    list_destroy(for_cond_exp);
    return ret_code;
}
