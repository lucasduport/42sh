#define _POSIX_C_SOURCE 200809L

#include "builtins.h"

#include <stdlib.h>

int builtin_cd(struct list *list,  __attribute__((unused)) struct environment *env)
{
    char *pwd = getenv("PWD"); 
    char *currpath;

    // rule 1
    if (list->next == NULL && getenv("HOME") == NULL)
        return 1;
    
    // rule 2
    if (list->next == NULL)
        list_append(&list, strdup(getenv("HOME")));

    list = list->next;
    currpath = list->current;

    // cd -    
    if (!strcmp(list->current, "-"))
    {
        currpath = getenv("OLDPWD");
        printf("%s\n", currpath);
    }
    if (list->current[0] == '~')
        currpath = getenv("HOME");

    if (chdir(currpath) == -1)
    {
        fprintf(stderr, "cd: no such file or directory: %s\n", currpath);
        return 1;
    }

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    setenv("PWD", cwd, 1);
    setenv("OLDPWD", pwd, 1);

    return 0;
}
