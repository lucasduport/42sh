#include "execute.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

int execute_command(struct ast *command, struct environment *env)
{
    if (command->arg == NULL)
        return 2;

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
    fflush(stdout);
    fflush(stderr);
    return code;
}
