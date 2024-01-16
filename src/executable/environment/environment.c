#include "environment.h"

struct environment *environment_new(void)
{
    struct environment *env = calloc(1, sizeof(struct environment));
    env->fd_out = 1;
    env->fd_err = 2;
    return env;
}

struct environment *dup_environment(struct environment *env)
{
    struct environment *new_env = calloc(1, sizeof(struct environment));
    new_env->fd_in = env->fd_in;
    new_env->fd_out = env->fd_out;
    new_env->fd_err = env->fd_err;
    new_env->variables = dup_variables(env->variables);
    return new_env;
}
