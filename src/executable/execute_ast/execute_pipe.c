#include "execute.h"

#include <err.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Exec each part of pipe
 * @param ast Current ast to execute
 * @param left 1 if it's left part of pipe 0 otherwise
 * @param fds_pipe Array that contain file descriptors of pipe
 * @param env Current environement
 * 
 * @return Return code of ast's execution
*/
static int exec_fork(struct ast *ast, int is_left, int fds_pipe[2], struct environment *env)
{
    pid_t p = fork();
    if (p == -1)
        errx(1, "fork failed");

    if (p == 0)
    {
        int fd_to_replace = (is_left ? STDOUT_FILENO : STDIN_FILENO);
        int good_fd = (is_left ? fds_pipe[1] : fds_pipe[0]);

        debug_printf(LOG_EXEC, "[EXEC] fdreplace = %d | good_fd = %d\n", fd_to_replace, good_fd);
        if (dup2(good_fd, fd_to_replace) == -1)
            errx(1, "dup2 failed");

        close(is_left ? fds_pipe[0] : fds_pipe[1]);

        int code = execute_ast(ast, env);
        exit(code);
    }

    return p;
}

int execute_pipe(struct ast *ast, struct environment *env)
{
    if (ast->first_child == NULL || ast->first_child->next == NULL)
    {
        debug_printf(LOG_EXEC,
                        "[EXECUTE] Missing left or right node for 'pipe' node\n");
        return -1;
    }

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
