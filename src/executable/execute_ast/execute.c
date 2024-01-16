#include "execute.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or then for 'if' node\n");
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
 * @brief Execute commands which are not builtins
 *
 * @param arg Chained list of arguments
 * @return return value from execution of the command
 */
static int execvp_wrapper(struct list *arg)
{
    int argc = 0;
    if (arg == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] execvp_wrapper: argv allocation failed\n");
        return -1;
    }

    int pid = fork();
    if (pid == 0)
    {
        // Child process
        size_t i = 0;
        // Rebuilds argc and argv
        char **argv = NULL;
        while (arg != NULL)
        {
            argv = realloc(argv, sizeof(char *) * (argc + 2));
            if (argv == NULL)
            {
                debug_printf(LOG_EXEC,
                             "[EXECUTE] execvp_wrapper: argv realloc failed\n");
                return -1;
            }
            argc++;
            argv[i] = arg->current;
            arg = arg->next;
            i++;
        }
        argv[i] = NULL;

        int retCode = execvp(argv[0], argv);
        if (retCode == -1 && errno == ENOENT)
        {
            fprintf(stderr, "%s: command not found\n", argv[0]);
            retCode = 127;
        }
        free(argv);
        return retCode;
    }
    else
    {
        // Parent process
        int parent_pid;
        // Wait for child process to finish
        waitpid(pid, &parent_pid, 0);
        // Return child process return value
        return WEXITSTATUS(parent_pid);
    }
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
        //expansion(command->arg);
        return execvp_wrapper(command->arg);
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
