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

int check_special_variable(const char *name)
{
    char *reserved_special_var[] = { "?", "@", "*", "#", "$", "UID", "RANDOM" };

    for (size_t i = 0; i < sizeof(reserved_special_var) / sizeof(char *); i++)
    {
        if (strcmp(name, reserved_special_var[i]) == 0)
            return 1;
    }
    return 0;
}

void set_exit_variale(struct environment *env, int return_code)
{
    char str[20];
    sprintf(str, "%d", return_code);
    set_variable(&env->variables, "?", str);
}

static void string_concat(char **str1, char *str2)
{
    if (str2 == NULL)
    {
        return;
    }

    size_t len1 = strlen(*str1);
    size_t len2 = strlen(str2);
    *str1 = realloc(*str1, (len1 + len2 + 2) * sizeof(char));

    if (str1 == NULL)
    {
        exit(EXIT_FAILURE);
    }

    strcat(*str1, " ");
    strcat(*str1, str2);
}

void set_star_variable(struct environment *env)
{
    int args_count = atoi(get_value(env->variables, "#"));
    if (args_count == 0)
        return;

    char i_str[20];
    char *star_value = strdup(get_value(env->variables, "1"));

    int i = 2;
    while (i <= args_count)
    {
        sprintf(i_str, "%d", i);
        char *str = get_value(env->variables, i_str);
        if (str != NULL)
            string_concat(&star_value, str);

        i++;
    }
    set_variable(&env->variables, "*", star_value);
    free(star_value);
}

struct list *get_at_variable(struct environment *env)
{
    int args_count = atoi(get_value(env->variables, "#"));
    if (args_count == 0)
        return NULL;

    char *star_var = strdup(get_value(env->variables, "*"));

    char *token = strtok(star_var, " ");
    struct list *list = NULL;

    while (token != NULL)
    {
        list_append(&list, strdup(token));
        token = strtok(NULL, " ");
    }

    return list;
}

void set_dollar_dollar(struct environment *env)
{
    char str[20];
    pid_t pid = getpid();
    sprintf(str, "%d", pid);
    set_variable(&env->variables, "$", str);
}

void set_uid(struct environment *env)
{
    char str[20];
    int uid = getuid();
    sprintf(str, "%d", uid);
    set_variable(&env->variables, "UID", str);
}

void set_random(struct environment *env)
{
    srand(time(NULL));
    int random_number = rand() % 32768;
    char str[20];
    sprintf(str, "%d", random_number);
    set_variable(&env->variables, "RANDOM", str);
}

void set_number_variable(struct environment *env, int argc, char *argv[])
{
    int i = 0;
    int loop = 1;

    if (strcmp(argv[1], "-c") == 0)
    {
        if (argc <= 4)
            loop = 0;
        else
            i = 4;
    }
    else
    {
        if (argc <= 2)
            loop = 0;
        else
            i = 2;
    }

    int var_number = 1;
    while (argv[i] != NULL && loop)
    {
        char var_name[20];
        sprintf(var_name, "%d", var_number);
        set_variable(&env->variables, var_name, argv[i]);
        var_number++;
        i++;
    }

    var_number--;
    char args_count[20];
    sprintf(args_count, "%d", var_number);
    set_variable(&env->variables, "#", args_count);
}

void set_environment(struct environment *env, int argc, char *argv[])
{
    if (argc)
        argc = argc;

    if (argv)
        argv = argv;

    // set_number_variable(env, argc, argv);

    // set_star_variable(env);

    set_exit_variale(env, 0);

    set_dollar_dollar(env);

    set_random(env);

    set_uid(env);

    // print_variables(env->variables);
}
