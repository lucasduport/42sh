#include "execute.h"

#include <fcntl.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

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

/**
 * @brief Open a file with the appropriate mode based on the operator used.
 * 
 * @return A redirection structure that contain the file descriptor and other usefull things.
 */
static struct redirection open_file(char *operator, char *filename)
{
    struct redirection redir;
    redir.io_number = 1;
    int flags = -1;

    if (!strcmp(operator, ">>"))
        flags = O_WRONLY | O_CREAT | O_APPEND;
    else if (!strcmp(operator, "<>"))
    {  
        redir.io_number = 0;
        flags = O_RDWR | O_CREAT | O_TRUNC;
    }
    else
    {  
        char *write_only[] = { ">", ">|", ">&" };
        for (size_t i = 0; i < sizeof(write_only) / sizeof(char *); i++)
        {
            if (!strcmp(write_only[i], operator))
            {
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                break;
            }
        }
        
        char *read_only[] = { "<", "<&" };
        for (size_t i = 0; i < sizeof(read_only) / sizeof(char *); i++)
        {
            if (!strcmp(read_only[i], operator))
            {
                redir.io_number = 0;
                flags = O_RDONLY;
                break;
            }
        }
   }

    if (flags == -1)
        debug_printf(LOG_EXEC, "operator '%s' not found\n", operator);
  
    redir.word_fd = open(filename, flags);
    return redir;
}

int execute_redir(struct ast *ast, struct environment *env)
{       
    if (ast->arg == NULL)
        return -1;

    char *operator = ast->arg->current;
    char *filename = ast->arg->next->current;

    struct redirection redir = open_file(operator, filename);

    if (ast->arg->next->next != NULL)
        redir.io_number = atoi(ast->arg->next->next->current);

    int save_fd = dup(redir.io_number);
    if (save_fd == -1)
    {
        debug_printf(LOG_EXEC, "redir: dup failed\n");
        return -1;
    }

    if (dup2(redir.word_fd, redir.io_number) == -1)
    {
        debug_printf(LOG_EXEC, "redir: dup2 failed\n");
        close(save_fd);
        return -1;
    }

    int code = execute_ast(ast->first_child, env);

    dup2(save_fd, redir.io_number);
    close(save_fd);
    close(redir.word_fd);
 
    return code;
}
