#include "builtins.h"

int alias(struct list *arguments, struct environment *env)
{
    if (arguments->next == NULL)
    {
        print_aliases(env);
        return 0;
    }
    else
    {
        int ret_code = 0;
        for (struct list *it = arguments->next; it != NULL; it = it->next)
        {
            char *arg = it->current;
            char *alias = strtok(arg, "=");
            char *value = strtok(NULL, "=");
            if (value == NULL)
            {
                if (print_alias(env, alias) == -1)
                {
                    fprintf(stderr, "alias: %s: not found\n", alias);
                    ret_code = 1;
                }
            }
            else
            {
                set_alias(env, alias, value);
            }
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

/**
 * @brief Print all aliases
 * 
 * @param env  Environment
 * @param alias Alias to print
 * @return int 0 if success, -1 otherwise
 */
static int print_alias(struct environment *env, char *alias)
{
    for (struct variable *it = env->future_aliases; it != NULL; it = it->next)
    {
        if (strcmp(it->name, alias) == 0)
        {
            printf("%s='%s'\n", it->name, it->value);
            return 0;
        }
    }
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

/**
 * @brief Add an alias to the list in alphabetical order
 * 
 * @param head Head of the list
 * @param name Name of the alias
 * @param value Value of the alias
 * @return struct variable* New head of the list
 */
static struct variable *add_alias(struct variable **aliases, char *name, char *value)
{
    struct variable *new = malloc(sizeof(struct variable));
    new->name = strdup(name);
    new->value = strdup(value);
    new->next = NULL;

    if (head == NULL)
        return new;

    struct variable *it = head;
    struct variable *prev = NULL;
    while (it != NULL && strcmp(it->name, name) < 0)
    {
        prev = it;
        it = it->next;
    }

    if (prev == NULL)
    {
        new->next = head;
        return new;
    }
    else
    {
        prev->next = new;
        new->next = it;
        return head;
    }
}
