#include "execute.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

/**
 * @brief Execute if node
 *
 * @param first_child First child of 'if' node -> condition
 * @return value of 'then' execution or 'else' execution or 0 by default
 */
static int execute_if(struct ast *first_child, struct environment *env)
{
    if (first_child == NULL || first_child->next == NULL)
    {
        debug_printf(LOG_EXEC,
                     "[EXECUTE] Missing condition or then for 'if' node\n");
        return -1;
    }

    // If condition is met
    int res_cond = execute_ast(first_child, env);
    if (res_cond == -1)
        return -1;
    else if (!res_cond)
        return execute_ast(first_child->next, env);

    // If there is no 'else' node
    if (first_child->next->next != NULL)
        return execute_ast(first_child->next->next, env);

    return 0;
}

/**
 * @brief Execute list node
 *
 * @param first_child First child of 'list' node
 * @return return value from execution of last command
 */
static int execute_list(struct ast *first_child, struct environment *env)
{
    struct ast *tmp = first_child;
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
    fflush(stdout);
    fflush(stderr);
    return code;
}

int execute_ast(struct ast *ast, struct environment *env)
{
    if (ast == NULL)
        return 0;

    if (ast->type == AST_IF)
        return execute_if(ast->first_child, env);

    else if (ast->type == AST_LIST)
        return execute_list(ast->first_child, env);

    else if (ast->type == AST_WHILE)
        return execute_while(ast->first_child, env);

    else if (ast->type == AST_UNTIL)
        return execute_until(ast->first_child, env);

    else if (ast->type == AST_NEG)
        return !execute_ast(ast->first_child, env);
    
    else if (ast->type == AST_AND || ast->type == AST_OR)
        return execute_and_or(ast, env);

    else if (ast->type == AST_PIPE)
        return execute_pipe(ast, env);
    
    else if (ast->type == AST_REDIR)
        return execute_redir(ast, env);
         
    else
        return execute_command(ast, env);
}
