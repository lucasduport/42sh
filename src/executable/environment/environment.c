#include "environment.h"

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

struct environment *environment_new(void)
{
    struct environment *env = calloc(1, sizeof(struct environment));
    return env;
}

struct environment *dup_environment(struct environment *env)
{
    print_variables(env->variables);
    struct environment *new_env = calloc(1, sizeof(struct environment));
    new_env->variables = dup_variables(env->variables);
    return new_env;
}

void environment_free(struct environment *env)
{
    free_variables(env->variables);
    free(env);
}

int check_env_variable(const char *name)
{
    char *reserved_env_var[] = { "OLDPWD", "PWD", "IFS" };

    for (size_t i = 0; i < sizeof(reserved_env_var) / sizeof(char *); i++)
    {
        if (strcmp(name, reserved_env_var[i]) == 0)
            return 1;
    }
    return 0;
}

void set_environment(struct environment *env, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // set_number_variable(env, argc, argv);

    // set_star_variable(env);
    if (argc >= 2 && strcmp(argv[1], "-c") == 0)
    {
        env->is_command = 1;
    }
    set_variable(env, "#", "0");

    // set_variable(&env->variables, "IFS", " \t\n");

    set_exit_variable(env, 0);

    set_dollar_dollar(env);

    set_random(env);

    set_uid(env);

    // print_variables(env->variables);
}

int set_error_value(struct environment *env, enum type_error type, int ret_code)
{
    env->error = type;
    return ret_code;
}
