#include "execute.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

exec_ast_node executes[] = { [AST_IF] = execute_if,
                             [AST_LIST] = execute_list,
                             [AST_WHILE] = execute_while,
                             [AST_UNTIL] = execute_until,
                             [AST_AND] = execute_and,
                             [AST_OR] = execute_or,
                             [AST_PIPE] = execute_pipe,
                             [AST_REDIR] = execute_redir,
                             [AST_COMMAND] = execute_command,
                             [AST_NEG] = execute_neg,
                             [AST_ASSIGNMENT] = execute_assignment,
                             [AST_FOR] = execute_for };

int execute_assignment(struct ast *ast, struct environment *env)
{
    if (ast->arg == NULL)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Missing command argument\n");
        return -1;
    }

    // Expand child before assignment
    if (ast->first_child != NULL)
    {
        int return_code = 0;
        struct list *expand_child_arg = expansion(ast->first_child->arg, env, &return_code);
        if (expand_child_arg == NULL)
            return return_code;

        list_destroy(ast->first_child->arg);
        ast->first_child->arg = expand_child_arg;
        ast->first_child->is_expand = 1;
    }

    char delim[] = "=";
    for (struct list *p = ast->arg; p != NULL; p = p->next)
    {
        // Set variable
        char *variable_name = strtok(p->current, delim);
        char *variable_value = strtok(NULL, delim);

        if (variable_name == NULL || variable_value == NULL)
        {
            fprintf(stderr, "Assignment failed\n");
            return 2;
        }

        if (!check_special_variable(variable_name))
        {
            if (set_variable(&env->variables, variable_name, variable_value)
                == -1)
            {
                fprintf(stderr, "Assignment failed\n");
                return 2;
            }
        }
    }

    execute_ast(ast->first_child, env);
    return 0;
}

int execute_list(struct ast *ast, struct environment *env)
{
    struct ast *tmp = ast->first_child;
    int res = 0;

    // While there is no error on our part
    while (tmp != NULL && res != -1 && res != 2)
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
