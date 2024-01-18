#include "execute.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

exec_ast_node executes[] = 
{
    [AST_IF] = execute_if, [AST_LIST] = execute_list, [AST_WHILE] = execute_while, [AST_UNTIL] = execute_until,
    [AST_AND] = execute_and, [AST_OR] = execute_or, [AST_PIPE] = execute_pipe, [AST_REDIR] = execute_redir,
    [AST_COMMAND] = execute_command, [AST_NEG] = execute_neg, [AST_ASSIGNMENT] = execute_assignment
};

int execute_assignment(struct ast *ast, struct environment *env)
{
    if (ast->arg == NULL)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Missing command argument\n");
        return -1;
    }

    char delim[] = "=";
    char *variable_name = strtok(ast->arg->current, delim);
    char *variable_value = strtok(NULL, delim);

    if (set_variable(&env->variables, variable_name, variable_value) == -1)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Set variable failed\n");
        fprintf(stderr, "Assignment failed\n");
        return -1;
    }
    return 0;
}

int execute_list(struct ast *ast, struct environment *env)
{
    struct ast *tmp = ast->first_child;
    int res = 0;
    
    // While there is no error on our part
    while (tmp != NULL && res != -1)
    {
        res = execute_ast(tmp, env);
        tmp = tmp->next;
    }

    return res;
}

/**
 * @brief Execute while node
 *
 * @param first_child First child of 'while' node -> condition
 * @return return value from execution of last command, 0 if nothing is execute
 */
static int execute_while(struct ast *first_child, struct environment *env)
{
    debug_printf(LOG_EXEC, "[EXECUTE] In execute while\n");
    if (first_child == NULL || first_child->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or do for 'while' node\n");
        return -1;
    }

    int ret_code = 0;
    while (!execute_ast(first_child, env))
    {
        debug_printf(LOG_EXEC, "[EXEC] In while loop\n");
        ret_code = execute_ast(first_child->next, env);
    }

    return ret_code;
}

/**
 * @brief Execute until node
 *
 * @param first_child First child of 'until' node -> condition
 * @return return value from execution of last command, 0 if nothing is execute
 */
static int execute_until(struct ast *first_child, struct environment *env)
{
    debug_printf(LOG_EXEC, "[EXECUTE] In execute until\n");
    if (first_child == NULL || first_child->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or do for 'until' node\n");
        return -1;
    }

    int ret_code = 0;
    while (execute_ast(first_child, env))
        ret_code = execute_ast(first_child->next, env);

    return ret_code;
}

/**
 * @brief Execute 'and' or 'or' node
 *
 * @param ast Current ast
 * @return return value from execution
 */
static int execute_and_or(struct ast *ast, struct environment *env)
{
    if (ast->first_child == NULL || ast->first_child->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing one commande for '%s' node\n", (ast->type == AST_AND) ? "and" : "or");
        return -1;
    }
    
    if (ast->type == AST_AND)
    {
        int code_left = execute_ast(ast->first_child, env);

        //Failed with error or success but with return code 1
        if (code_left != 0)
            return code_left;
        
        int code_right = execute_ast(ast->first_child->next, env);
        return code_right;
    }
    else
    {
        int code_left = execute_ast(ast->first_child, env);

        //Failed with error or success but with return code 1
        if (code_left == 0 || code_left == -1)
            return code_left;
        
        int code_right = execute_ast(ast->first_child->next, env);
        return code_right;
    }
}

/**
 * @brief Execute command node
 *
 * @param command Node command
 * @return return value from execution of the command
 */
static int execute_command(struct ast *command, struct environment *env)
{
    if (command->arg == NULL)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Missing command argument\n");
        return -1;
    }

    if (expansion(command->arg, env) == -1)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Expansion failed\n");
        fprintf(stderr, "Expansion failed\n");
        return -1;
    }

    // First arg contains the command
    char *first_arg = list_get_n(command->arg, 0);

    int code = 0;
    if (strcmp(first_arg, "echo") == 0)
        code = builtin_echo(command->arg);

    else if (strcmp(first_arg, "true") == 0)
        code = builtin_true(command->arg);

    else if (strcmp(first_arg, "false") == 0)
        code = builtin_false(command->arg);
    else
        code = execvp_wrapper(command->arg, env);
    //FIXME: we should only fflush for builtins
    fflush(stdout);
    fflush(stderr);
    return code;
}

int execute_ast(struct ast *ast, struct environment *env)
{
    if (ast == NULL)
        return 0;

    return executes[ast->type](ast, env);
}
