#define _POSIX_C_SOURCE 200112L

#include "builtins.h"

#include <stdlib.h>

static int is_dot(char *path)
{
    size_t i = 0;
    char *dot = ".";

    while (path[i] != '\0' && path[i] != '/')
    {
        if (path[i] != dot[i])
            return 0; 
    }

    return 1;
}

static int is_dotdot(char *path)
{    
    size_t i = 0;
    char *dot = "..";

    while (path[i] != '\0' && path[i] != '/')
    {
        if (path[i] != dot[i])
            return 0; 
    }

    return 1;
}

int builtin_cd(struct list *list)
{
    char *pwd = getenv("PWD"); 
    char *currpath;

    // rule 1
    if (list->next == NULL && getenv("HOME") == NULL)
        return -1;
    
    // rule 2
    if (list->next == NULL)
        list_append(&list, getenv("HOME"));

    list = list->next;

    // rule 3 - 6
    currpath = list->current;  

    // rule 8
    // TODO

    chdir(currpath);
    setenv("OLDPWD", currpath, 1);

    return 0;
}
