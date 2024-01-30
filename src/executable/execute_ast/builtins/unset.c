#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "builtins.h"

struct option
{
    int v;
    int f;
    int all;
};

/**
 * @brief Check if name doesn't contain special char
 *
 * @return 1 if contains, 0 otherwise
 */
static int check_special_name(const char *name)
{
    char special_char[] = { '$', '?', '@', '*', '#', '!', '.', '-', '+' };

    for (size_t i = 0; name[i] != '\0'; i++)
    {
        for (size_t j = 0; j < sizeof(special_char) / sizeof(char); j++)
        {
            if (name[i] == special_char[j])
                return 1;
        }
    }
    return 0;
}

/**
 * @brief Set the option struct for unset builtin
 *
 * @param str
 * @param options
 */
static void parse_option(char *str, struct option *options)
{
    if (strcmp(str, "-v") == 0)
        options->v = 1;
    else if (strcmp(str, "-f") == 0)
        options->f = 1;
    else
        options->all = 1;
}

/**
 * @brief Unset a variable
 *
 * @return 1 if it works, 0 otherwise
 */
static int unset_variable(struct environment *env, struct option *options,
                          char *name)
{
    if (strcmp(name, "UID") == 0)
    {
        fprintf(stderr, "unset: Cannot unset UID\n");
        return 1;
    }

    if (check_special_name(name) && options->all == 0)
    {
        fprintf(stderr, "unset: %s: not a valid identifier\n", name);
        return 1;
    }

    if (exist_variables(env->variables, name))
    {
        set_variable(env, name, "");
    }
    else if (check_env_variable(name))
    {
        setenv(name, "", 1);
    }
    return 0;
}

/**
 * @brief Unset a function
 */
static int unset_function(struct environment *env, char *name)
{
    if (exist_functions(env->functions, name))
        delete_function(&env->functions, name);
    return 0;
}

int builtin_unset(struct list *list, struct environment *env)
{
    struct list *n = list->next;
    if (n == NULL)
        return 0;

    if (strcmp(n->current, "-vf") == 0 || strcmp(n->current, "-fv") == 0)
    {
        fprintf(stderr, "unset: -v and -f cannot be used together\n");
        return 1;
    }

    if (strcmp(n->current, "-v") != 0 && strcmp(n->current, "-f") != 0
        && n->current[0] == '-')
    {
        fprintf(stderr, "unset: %s invalid option\n", n->current);
        return 2;
    }

    struct option *options = calloc(1, sizeof(struct option));
    parse_option(n->current, options);

    int code = 0;

    if (options->all == 0)
        n = n->next;

    while (n != NULL)
    {
        if (options->v)
        {
            int newcode = unset_variable(env, options, n->current);
            code = newcode != 0 ? newcode : code;
        }
        else if (options->f)
        {
            int newcode = unset_function(env, n->current);
            code = newcode != 0 ? newcode : code;
        }
        else
        {
            int newcode = unset_variable(env, options, n->current);
            code = newcode != 0 ? newcode : code;
            newcode = unset_function(env, n->current);
            code = newcode != 0 ? newcode : code;
        }
        n = n->next;
    }
    free(options);
    return code;
}
