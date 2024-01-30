#include "execute.h"

#include <fnmatch.h>

int execute_if(struct ast *ast, struct environment *env)
{
    ast = ast->first_child;

    // If condition is met
    int res_cond = execute_ast(ast, env);
    if (env->error != NO_ERROR)
        return res_cond;
    else if (!res_cond)
        return execute_ast(ast->next, env);

    // If there is no 'else' node
    if (ast->next->next != NULL)
        return execute_ast(ast->next->next, env);

    return 0;
}

int execute_and(struct ast *ast, struct environment *env)
{
    int code_left = execute_ast(ast->first_child, env);

    // Failed with error or success but with return code 1
    if (code_left != 0)
        return code_left;

    int code_right = execute_ast(ast->first_child->next, env);
    return code_right;
}

int execute_or(struct ast *ast, struct environment *env)
{
    int code_left = execute_ast(ast->first_child, env);

    // Failed with error or success but with return code 1
    if (code_left == 0)
        return code_left;

    int code_right = execute_ast(ast->first_child->next, env);
    return code_right;
}

int execute_neg(struct ast *ast, struct environment *env)
{
    int code = execute_ast(ast->first_child, env);
    return !code;
}

int execute_case(struct ast *ast, struct environment *env)
{
    int ret_code = 0;
    char *word = expand_string(ast->arg->current, env, &ret_code);
    if (ret_code != 0)
        return set_error(env, STOP, ret_code);
    
    for (struct ast *c = ast->first_child; c != NULL; c = c->next)
    {
        struct list *item_exp = expansion(c->arg, env, &ret_code);
        if (ret_code != 0)
            return set_error(env, STOP, ret_code);
        
        for (struct list *i = item_exp; i != NULL; i = i->next)
        {
            if (fnmatch(i->current, word, 0) == 0)
            {
                list_destroy(item_exp);
                free(word);
                return execute_ast(c->first_child, env);
            }
        }
        list_destroy(item_exp);
    }
    free(word);
    return 0;
}
