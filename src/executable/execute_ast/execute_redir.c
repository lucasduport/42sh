#include "execute.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

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
        flags = O_RDWR;
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
  
    redir.word_fd = open(filename, flags, 0664);
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
