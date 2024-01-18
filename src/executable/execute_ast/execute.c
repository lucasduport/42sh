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

int execute_ast(struct ast *ast, struct environment *env)
{
    if (ast == NULL)
        return 0;

    return executes[ast->type](ast, env);
}
