#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execute.h"

/**
 * @brief Exec each part of pipe
 * 
 * @param ast Current ast to execute
 * @param left 1 if it's left part of pipe 0 otherwise
 * @param fds_pipe Array that contain file descriptors of pipe
 * @param env Current environement
 *
 * @return Return code of ast's execution
 */
static int exec_fork(struct ast *ast, int is_left, int fds_pipe[2],
                     struct environment *env)
{
    pid_t p = fork();
    if (p == -1)
        errx(1, "fork failed");

    if (p == 0)
    {
        int fd_to_replace = (is_left ? STDOUT_FILENO : STDIN_FILENO);
        int good_fd = (is_left ? fds_pipe[1] : fds_pipe[0]);

        if (dup2(good_fd, fd_to_replace) == -1)
            errx(1, "dup2 failed");

        close(is_left ? fds_pipe[0] : fds_pipe[1]);

        int code = execute_ast(ast, env);
        _exit(code);
    }

    return p;
}

int execute_pipe(struct ast *ast, struct environment *env)
{
    // Create pipe
    int fds_pipe[2];
    if (pipe(fds_pipe) == -1)
    {
        fprintf(stderr, "Create pipe failed\n");
        return -1;
    }

    // Exec each part with fork
    int p_left = exec_fork(ast->first_child, 1, fds_pipe, env);
    int p_right = exec_fork(ast->first_child->next, 0, fds_pipe, env);

    // Wait process
    int returnStatus;
    waitpid(p_left, &returnStatus, 0);
    close(fds_pipe[1]);
    waitpid(p_right, &returnStatus, 0);
    close(fds_pipe[0]);

    // Return exit code from second command
    return WEXITSTATUS(returnStatus);
}

/**
 * @brief Open a file with the appropriate mode based on the operator used.
 *
 * @return A redirection structure that contain the file descriptor and other
 * usefull things.
 */
static struct redirection open_file(char *operator, char * filename)
{
    struct redirection redir;

    int index = 0;
    int default_io[] = { 1, -1, 0, -1, 1, 1, 1, -1, 0, 0 };
    char *operators[] = { ">>", NULL, "<>", NULL, ">",
                          ">|", ">&", NULL, "<",  "<&" };
    
    // FIXME J'ai delete le flag O_APPEND pour <>
    int flags[] = { O_WRONLY | O_CREAT | O_APPEND, O_RDWR | O_CREAT,
                    O_WRONLY | O_CREAT | O_TRUNC, O_RDONLY };

    size_t i = 0;
    for (; i < sizeof(operators) / sizeof(char *); i++)
    {
        if (operators[i] == NULL)
            index++;

        else if (!strcmp(operators[i], operator))
            break;
    }

    if (i == sizeof(operators) / sizeof(char *))
        debug_printf(LOG_EXEC, "[EXEC] operator '%s' not found\n", operator);

    if (i == 6 || i == 9)
        redir.word_fd = atoi(filename);
    else
        redir.word_fd = open(filename, flags[index], 0644);

    redir.io_number = default_io[i];
    return redir;
}

int execute_redir(struct ast *ast, struct environment *env)
{
    if (ast->first_child->type == AST_ASSIGNMENT)
        return execute_ast(ast->first_child, env);

    struct list *arg_expand = ast->arg;
    if (!ast->is_expand)
    {
        int ret = 0;
        arg_expand = expansion(ast->arg, env, &ret);
        if (ret != 0)
            return set_error(env, STOP, ret);
    }

    char *operator= arg_expand->current;
    char *filename = arg_expand->next->current;

    struct redirection redir = open_file(operator, filename);

    if (arg_expand->next->next != NULL)
    {
        int fd = atoi(ast->arg->next->next->current);
        if (fcntl(fd, F_GETFD) != -1)
            redir.io_number = fd;
    }

    int save_fd = dup(redir.io_number);
    if (save_fd == -1)
        goto error;

    if (dup2(redir.word_fd, redir.io_number) == -1)
    {
        close(save_fd);
        goto error;
    }

    int code = execute_ast(ast->first_child, env);

    dup2(save_fd, redir.io_number);
    close(save_fd);
    close(redir.word_fd);

    if (!ast->is_expand)
        list_destroy(arg_expand);
    return code;

error:
    if (!ast->is_expand)
        list_destroy(arg_expand);
    fprintf(stderr, "redir: dup2 failed\n");
    return set_error(env, CONTINUE, 1);
}
