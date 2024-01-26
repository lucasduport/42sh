#include "execute.h"

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
