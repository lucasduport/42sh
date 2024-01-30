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
        return -1;

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
                return -1;
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

exec_builtins builtins[] = { builtin_echo,  builtin_true,   builtin_false,
                             builtin_exit,  builtin_export, builtin_continue,
                             builtin_break, builtin_dot,    builtin_unset,
                             builtin_alias };

int execute_command(struct ast *ast, struct environment *env)
{
    struct list *tmp_arg = ast->arg;
    if (!ast->is_expand)
    {
        int return_code = 0;
        tmp_arg = expansion(ast->arg, env, &return_code);
        if (tmp_arg == NULL)
            return set_error(env, STOP, return_code);
    }
    ast->is_expand = !ast->is_expand;

    // First arg contains the command
    char *first_arg = list_get_n(tmp_arg, 0);
    int code = 0;

    // Check if it's a function
    struct ast *f = get_function(env, first_arg);
    if (f != NULL)
    {
        struct variable *tmp_var = dup_variables(env->variables);
        set_number_variable(env, tmp_arg->next);

        code = execute_ast(f, env);

        free_variables(env->variables);
        env->variables = tmp_var;
        goto goodbye;
    }

    // Check if it's builtin
    char *builtins_name[] = { "echo",   "true",     "false", "exit",
                              "export", "continue", "break", ".",
                              "unset",  "alias",    NULL };
    for (int i = 0; builtins_name[i] != NULL; i++)
    {
        if (strcmp(first_arg, builtins_name[i]) == 0)
        {
            code = builtins[i](tmp_arg, env);
            goto goodbye;
        }
    }

    // If it's an alias, replace its value in the current string
    alias_expansion(env->aliases, &ast->arg->current);

    // If it's neither a function nor a builtin
    code = execvp_wrapper(tmp_arg, env);

goodbye:
    fflush(stderr);
    fflush(stdout);
    if (ast->is_expand)
        list_destroy(tmp_arg);
    ast->is_expand = !ast->is_expand;

    set_exit_variable(env, code);
    return code;
}

int execute_subshell(struct ast *ast, struct environment *env)
{
    // Forks a new process and executes the ast in it
    int pid = fork();
    if (pid == -1)
        return 2;
    if (pid == 0)
    {
        // Child process
        struct environment *child_env = dup_environment(env);
        int code = execute_ast(ast->first_child, child_env);
        // Set exit variable of the ast to the current environment
        set_exit_variable(env, code);
        environment_free(child_env);
        _exit(code);
    }
    else
    {
        // Parent process
        int return_status;
        // Wait for child process to finish
        waitpid(pid, &return_status, 0);
        // Return child process return value
        return WEXITSTATUS(return_status);
    }
}

int execute_function(struct ast *ast, struct environment *env)
{
    return set_function(env, ast->arg->current, ast->first_child);
}
