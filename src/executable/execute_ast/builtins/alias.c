#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include <string.h>

#include "builtins.h"

/**
 * @brief Set an alias in future aliases
 *  Update the value if alias already exists, create the alias otherwise
 *
 * @param name Name of the alias
 * @param value Value of the alias
 */
static int set_alias(struct variable **list_aliases, char *name, char *value)
{
    // Check in all future aliases
    for (struct variable *it = *list_aliases; it != NULL; it = it->next)
    {
        // If found in future aliases
        if (strcmp(it->name, name) == 0)
        {
            // Remove actual value
            free(it->value);
            if (value != NULL)
            {
                it->value = strdup(value);
            }
            else
            {
                it->value = NULL;
            }
            return 0;
        }
    }

    // If the alias is not found, add it to the list
    return add_variable(list_aliases, name, value);
}

/**
 * @brief Check if an alias exists in the list
 *
 * @param list_aliases List of aliases
 * @param name Name of the alias to check
 * @return int 1 if the alias exists, 0 otherwise
 */
static int alias_exists(struct variable *list_aliases, char *name)
{
    for (struct variable *it = list_aliases; it != NULL; it = it->next)
    {
        if (strcmp(it->name, name) == 0)
            return 1;
    }
    return 0;
}

int builtin_alias(struct list *list, struct environment *env)
{
    // Set all aliases
    int ret_code = 0;
    for (struct list *it = list->next; it != NULL; it = it->next)
    {
        char *cpy = strdup(it->current);
        char *name = strtok(cpy, "=");
        if (name == NULL)
        {
            ret_code = 1;
            free(cpy);
            continue;
        }

        char *value = strtok(NULL, "");
        // If there is no value, juste print it
        if (value == NULL)
        {
            ret_code = 1;
        }
        else
            set_alias(&env->add_aliases, name, value);
        free(cpy);
    }
    return ret_code;
}

/**
 * @brief Delete an alias
 *
 * @param env  Environment
 * @param name  Name of the alias to delete
 * @return int  0 if the alias was deleted, -1 otherwise
 */
static int delete_alias(struct environment *env, char *name)
{
    int code = -1;
    struct variable *current = env->aliases;
    struct variable *previous = NULL;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            if (previous != NULL)
                previous->next = current->next;
            else
                env->aliases = current->next;

            free(current->name);
            free(current->value);
            free(current);
            code = 0;
            break;
        }
        previous = current;
        current = current->next;
    }
    return code;
}

int builtin_unalias(struct list *arguments, struct environment *env)
{
    if (arguments->next == NULL)
    {
        fprintf(stderr, "unalias: not enough arguments\n");
        return 1;
    }
    else
    {
        int ret = 0;
        for (struct list *it = arguments->next; it != NULL; it = it->next)
        {
            if (alias_exists(env->aliases, it->current)
                || alias_exists(env->add_aliases, it->current))
                set_alias(&env->add_aliases, it->current, NULL);
            else
            {
                fprintf(stderr, "unalias: %s: not found\n", it->current);
                ret = 1;
            }
        }
        return ret;
    }
}

void update_aliases(struct environment *env)
{
    struct variable *f = env->add_aliases;
    while (f != NULL)
    {
        if (f->value == NULL)
            delete_alias(env, f->name);
        else
            set_alias(&env->aliases, f->name, f->value);
        f = f->next;
    }
    free_variables(env->add_aliases);
    env->add_aliases = NULL;
}

void alias_expansion(struct variable *alias, char **string)
{
    char *base_string = strdup(*string);
    bool was_replaced = true;
    do
    {
        was_replaced = false;
        for (struct variable *it = alias; it != NULL; it = it->next)
        {
            if (strcmp(it->name, *string) == 0)
            {
                *string =
                    realloc(*string, (strlen(it->value) + 1) * sizeof(char));
                strcpy(*string, it->value);
                was_replaced = true;
            }
        }
    } while (was_replaced && strcmp(base_string, *string) != 0);
    free(base_string);
}
