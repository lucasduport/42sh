#include "execute.h"

int execute_if(struct ast *ast, struct environment *env)
{
    ast = ast->first_child;
    if (ast == NULL || ast->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or then for 'if' node\n");
        return -1;
    }

    // If condition is met
    int res_cond = execute_ast(ast, env);
    if (res_cond == -1)
        return -1;
    else if (!res_cond)
        return execute_ast(ast->next, env);

    // If there is no 'else' node
    if (ast->next->next != NULL)
        return execute_ast(ast->next->next, env);

    return 0;
}

int execute_and(struct ast *ast, struct environment *env)
{
    if (ast->first_child == NULL || ast->first_child->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing one commande for '%s' node\n", (ast->type == AST_AND) ? "and" : "or");
        return -1;
    }
    
    int code_left = execute_ast(ast->first_child, env);

    //Failed with error or success but with return code 1
    if (code_left != 0)
        return code_left;
        
    int code_right = execute_ast(ast->first_child->next, env);
    return code_right;
}

int execute_or(struct ast *ast, struct environment *env)
{
    if (ast->first_child == NULL || ast->first_child->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing one commande for '%s' node\n", (ast->type == AST_AND) ? "and" : "or");
        return -1;
    }
    
    int code_left = execute_ast(ast->first_child, env);

    //Failed with error or success but with return code 1
    if (code_left == 0 || code_left == -1)
        return code_left;
        
    int code_right = execute_ast(ast->first_child->next, env);
    return code_right;
}

int execute_neg(struct ast *ast, struct environment *env)
{
    int code = execute_ast(ast->first_child, env);
    if (code == 127 || code == -1)
        return code;
    return !code;
}
