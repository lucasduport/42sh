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

struct environment *check_env_variable(const char *name)
{
    char *reserved_env_var[] = {"OLDPWD", "PWD", "IFS"};

    for (size_t i = 0; i < sizeof(reserved_env_var) / sizeof(char *); i++)
    {
        if (strcmp(name, reserved_env_var[i]) == 0)
            return 1;
    }
    return 0;
}
