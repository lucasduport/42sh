#include "environment.h"

struct environment *environment_new(void)
{
    struct environment *env = calloc(1, sizeof(struct environment));
    return env;
}

struct environment *dup_environment(struct environment *env)
{
    struct environment *new_env = calloc(1, sizeof(struct environment));
    new_env->variables = dup_variables(env->variables);
    return new_env;
}
