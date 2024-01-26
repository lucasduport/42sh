#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execute.h"

/**
 * @brief Execute commands which are not builtins
 *
 * @param arg Chained list of arguments
 * @return return value from execution of the command
 */
static int execvp_wrapper(struct list *arg, struct environment *env)
{
    // Because unused variable
    if (env != NULL)
        *env = *env;

    int argc = 0;
    if (arg == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] execvp_wrapper: argv allocation failed\n");
        return -1;
    }

    int pid = fork();
    if (pid == -1)
        return 2;

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

        execvp(argv[0], argv);
        fprintf(stderr, "%s: command not found\n", argv[0]);
        _exit(127);
    }

    int return_status;
    // Wait for child process to finish
    waitpid(pid, &return_status, 0);
    // Return child process return value
    return WEXITSTATUS(return_status);
}

int execute_command(struct ast *ast, struct environment *env)
{
    struct list *tmp_arg = ast->arg;
    if (!ast->is_expand)
    {
        int return_code = 0;
        tmp_arg = expansion(ast->arg, env, &return_code);
        if (tmp_arg == NULL)
            return set_error_value(env, FAILED_EXPAND, return_code);
    }
    ast->is_expand = !ast->is_expand;

    // First arg contains the command
    char *first_arg = list_get_n(tmp_arg, 0);
    int code = 0;

    if (strcmp(first_arg, "echo") == 0)
        code = builtin_echo(tmp_arg);

    else if (strcmp(first_arg, "true") == 0)
        code = builtin_true(tmp_arg);

    else if (strcmp(first_arg, "false") == 0)
        code = builtin_false(tmp_arg);

    else if (strcmp(first_arg, "exit") == 0)
        code = builtin_exit(tmp_arg, env);

    else if (strcmp(first_arg, "export") == 0)
        code = builtin_export(tmp_arg, env);

    else if (strcmp(first_arg, "continue") == 0)
        code = builtin_continue(tmp_arg, env);
    
    else if (strcmp(first_arg, "break") == 0)
        code = builtin_break(tmp_arg, env);

    else if (strcmp(first_arg, ".") == 0)
        code = builtin_dot(tmp_arg, env);
    else
        code = execvp_wrapper(tmp_arg, env);

    fflush(stderr);
    fflush(stdout);
    // If we expand -> free tmp_arg
    if (ast->is_expand)
        list_destroy(tmp_arg);
    ast->is_expand = !ast->is_expand;

    set_exit_variable(env, code);
    return code;
}
