#include "execute.h"

#include <string.h>

/**
 * @brief Execute if node
 *
 * @param first_child First child of 'if' node -> condition
 * @return value of 'then' execution or 'else' execution or 0 by default
 */
static int execute_if(struct ast *first_child)
{
    if (first_child == NULL || first_child->next == NULL)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Missing condition or then for 'if' node\n");
        return -1;
    }

    // If condition is met
    int res_cond = execute_ast(first_child);
    if (res_cond == -1)
        return -1;
    else if (!res_cond)
        return execute_ast(first_child->next);

    // If there is no 'else' node
    if (first_child->next->next != NULL)
        return execute_ast(first_child->next->next);

    return 0;
}

/**
 * @brief Execute list node
 *
 * @param first_child First child of 'list' node
 * @return return value from execution of last command
 */
static int execute_list(struct ast *first_child)
{
    struct ast *tmp = first_child;
    int res = 0;
    // While there is no error on our part
    while (tmp != NULL && res != -1)
    {
        res = execute_ast(tmp);
        tmp = tmp->next;
    }
    return res;
}

/**
 * @brief Execute command node
 *
 * @param command Node command
 * @return return value from execution of the command
 */
static int execute_command(struct ast *command)
{
    if (command->arg == NULL)
    {
        debug_printf(LOG_EXEC, "[EXECUTE] Missing command argument\n");
        return -1;
    }

    // First arg contains the command
    char *first_arg = list_get_n(command->arg, 0);

    if (strcmp(first_arg, "echo") == 0)
        return builtin_echo(command->arg);

    else if (strcmp(first_arg, "true") == 0)
        return builtin_true(command->arg);

    else if (strcmp(first_arg, "false") == 0)
        return builtin_false(command->arg);

    else
    {
        fprintf(stderr, "unknown command : %s\n", first_arg);
        return 127;
    }
}

int execute_ast(struct ast *ast)
{
    if (ast == NULL)
        return 0;

    if (ast->type == AST_IF)
        return execute_if(ast->first_child);

    else if (ast->type == AST_LIST)
        return execute_list(ast->first_child);

    else
        return execute_command(ast);
}
