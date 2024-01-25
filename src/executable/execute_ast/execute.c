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
        return -1;

    char *variable_value = NULL;
    struct variable *var_before = NULL;
    int code = 0;

    struct ast *child = ast->first_child;
    // Expand child before assignment
    if (ast->first_child != NULL)
    {
        // If there is child => variable assignment is local
        var_before = dup_variables(env->variables);
        child = expand_ast(ast->first_child, env, &code);
        if (code == -1)
            return -1; 
    }

    // Do all assignments
    for (struct list *p = ast->arg; p != NULL; p = p->next)
    {
        // Set variable
        char *cpy = strdup(p->current);
        char *variable_name = strtok(p->current, "=");

        char *val = strstr(cpy, "=");
        val++;
        //debug_printf(LOG_LEX, "[EXECUTE] Variable name: '%s'\n", variable_name);
        //debug_printf(LOG_LEX, "[EXECUTE] Variable value: '%s'\n", variable_value);

        //variable_value = expand_string(strtok(NULL, "="), env, &code);
        variable_value = expand_string(val, env, &code);

        free(cpy);

        if (variable_value == NULL)
            code = 0;

        // If it's environment variable
        else
        {
            if ((code = set_variable(env, variable_name, variable_value)) != 0)
            {
                env->exit = 1;
                goto error;
            }
                
        }
        free(variable_value);
    }

    code = execute_ast(child, env);

    // If variables are local, reset environment
    if (var_before != NULL)
    {
        free_variables(env->variables);
        env->variables = var_before;
    }
    if (ast->first_child != NULL)
        ast_free(child);
    
    return code;

error:
    if (ast->first_child != NULL)
        ast_free(child);
    free(variable_value);
    free_variables(var_before);
    fprintf(stderr, "Assignment failed\n");
    return code;
}

int execute_list(struct ast *ast, struct environment *env)
{
    struct ast *tmp = ast->first_child;
    int res = 0;

    // While there is no error on our part
    while (tmp != NULL && res != -1 && res != 2 && !env->exit && !env->nb_continue && !env->nb_break)
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
