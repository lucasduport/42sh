#include "environment.h"

struct environment *environment_new(void)
{
    struct environment *env = malloc(sizeof(struct environment));
    env->fd_in = 0;
    env->fd_out = 1;
    env->fd_err = 2;

    env->variables = NULL;
    
    return env;
}

struct environment *environment_deep_copy(void)
{
    //TODO
}
