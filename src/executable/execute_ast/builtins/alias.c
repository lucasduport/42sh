#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include <string.h>

#include "builtins.h"

int exist_aliases(struct variable *head, const char *name)
{
    struct variable *current = head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
            return 1;
        current = current->next;
    }
    return 0;
}

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
            it->value = strdup(value);
            return 0;
        }
    }

    // If the alias is not found, add it to the list
    return add_variable(list_aliases, name, value);
}

/**
 * @brief Print an alias
 *
 * @param env  Environment
 * @param alias Name of the alias to print
 *
 * @return -1 if the alias was not found, 0 otherwise
 */
static int print_alias(struct environment *env, char *alias)
{
    // Search in future aliases in priority
    for (struct variable *it = env->future_aliases; it != NULL; it = it->next)
    {
        if (strcmp(it->name, alias) == 0)
        {
            printf("%s='%s'\n", it->name, it->value);
            return 0;
        }
    }

    // Otherwise search in current aliases
    for (struct variable *it = env->aliases; it != NULL; it = it->next)
    {
        if (strcmp(it->name, alias) == 0)
        {
            printf("%s='%s'\n", it->name, it->value);
            return 0;
        }
    }
    return -1;
}

/**
 * @brief Print all aliases in alphabetical order, including future aliases
 *  If an alias is in current and future lists, we write the value in future
 * list
 *
 * @param env Environment
 */
static void print_aliases(struct environment *env)
{
    struct variable *head = NULL;
    for (struct variable *it = env->aliases; it != NULL; it = it->next)
        set_alias(&head, it->name, it->value);

    for (struct variable *it = env->future_aliases; it != NULL; it = it->next)
        set_alias(&head, it->name, it->value);

    for (struct variable *it = head; it != NULL; it = it->next)
        printf("%s='%s'\n", it->name, it->value);

    free_variables(head);
}

int builtin_alias(struct list *list, struct environment *env)
{
    // Only alias word
    if (list->next == NULL)
    {
        print_aliases(env);
        return 0;
    }
    else
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
                continue;
            }

            char *value = strstr(cpy, "=");
            // If there is no value, juste print it
            if (value == NULL)
            {
                if (print_alias(env, name) == -1)
                {
                    fprintf(stderr, "alias: %s: not found\n", name);
                    ret_code = 1;
                }
            }
            else
                set_alias(&env->future_aliases, name, value);
        }
        return ret_code;
    }
}

static int delete_alias(struct environment *env, char *name)
{
    int code = -1;
    struct variable *future = env->future_aliases;
    struct variable *previous = NULL;
    while (future != NULL)
    {
        if (strcmp(future->name, name) == 0)
        {
            if (previous != NULL)
                previous->next = future->next;
            else
                previous = future->next;

            free(future->name);
            free(future->value);
            free(future);
            code = 0;
        }
        previous = future;
        future = future->next;
    }

    struct variable *current = env->aliases;
    struct variable *previous_c = NULL;
    while (future != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            if (previous_c != NULL)
                previous_c->next = current->next;
            else
                previous_c = current->next;

            free(current->name);
            free(current->value);
            free(current);
            code = 0;

        }
        previous_c = current;
        current = current->next;
    }
    return code;
}


int unalias(struct list *arguments, struct environment *env)
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
            if (delete_alias(env, it->current) == -1)
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
    struct variable *f = env->future_aliases;
    while (f != NULL)
    {
        set_alias(&env->aliases, f->name, f->value);
        f = f->next;
    }
    free_variables(env->future_aliases);
}

void alias_expansion(struct variable *alias, char **string)
{
    for (struct variable *it = alias; it != NULL; it = it->next)
    {
        if (strcmp(it->name, *string) == 0)
        {
            *string = realloc(*string, (strlen(it->value) + 1) * sizeof(char));
            strcpy(*string, it->value);
            return;
        }
    }
}
