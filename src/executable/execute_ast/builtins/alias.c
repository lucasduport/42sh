#include "builtins.h"

/**
 * @brief Set an alias in future aliases
 *  Update the value if alias already exists, create the alias otherwise
 * 
 * @param name Name of the alias
 * @param value Value of the alias
*/
static int set_alias(struct environment *env, const char *name, const char *value)
{
    // Check in all future aliases
    for (struct variable *it = env->future_aliases; it != NULL; it = it->next)
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
    return add_variable(&env->future_aliases, name, value);
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
 *  If an alias is in current and future lists, we write the value in future list
 * 
 * @param env Environment
 */
static void print_aliases(struct environment *env)
{
    struct variable *head = NULL;
    for (struct variable *it = env->aliases; it != NULL; it = it->next)
        head = add_alias(head, it->name, it->value);

    for (struct variable *it = env->future_aliases; it != NULL; it = it->next)
        head = add_alias(head, it->name, it->value);

    for (struct variable *it = head; it != NULL; it = it->next)
        printf("%s='%s'\n", it->name, it->value);

    free_variables(head);
}


int builtin_alias(struct list *list, struct environment *env)
{
    // Only alias word
    if (list->next == NULL)
    {
        print_aliases(list);
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
                set_alias(env, name, value);
        }
        return ret_code;
    }
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
            if (unset_alias(env, it->current) == -1)
            {
                fprintf(stderr, "unalias: %s: not found\n", it->current);
                ret = 1;
            }
        }
        return ret;
    }
}
