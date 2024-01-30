#include "environment.h"

int add_variable(struct variable **head, const char *name, char *value)
{
    struct variable *new_variable = calloc(1, sizeof(struct variable));
    if (new_variable == NULL)
        return -1;
    new_variable->name = strdup(name);
    new_variable->value = strdup(value);
    new_variable->next = *head;
    *head = new_variable;
    return 0;
}

int set_variable(struct environment *env, const char *name, char *value)
{
    if (strcmp(name, "UID") == 0)
    {
        fprintf(stderr, "Cannot set UID: ");
        return env->is_command ? 127 : 1;
    }
    struct variable *current = env->variables;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            free(current->value);
            current->value = strdup(value);
            return 0;
        }
        current = current->next;
    }
    if (check_env_variable(name))
    {
        int ret = 0;
        if (setenv(name, value, 1) == -1)
            ret = -1;
        return ret;
    }
    // If the variable is not found, add it to the list
    return add_variable(&env->variables, name, value);
}

char *get_value(struct environment *env, const char *name)
{
    const struct variable *current = env->variables;
    if (strcmp(name, "RANDOM") == 0)
        set_random(env);
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current->value;
        }
        current = current->next;
    }
    // Return NULL as a default value if the variable is not found
    return getenv(name);
}

struct variable *dup_variables(struct variable *head)
{
    struct variable *new_head = NULL;
    struct variable *current = head;
    while (current != NULL)
    {
        add_variable(&new_head, current->name, current->value);
        if (new_head == NULL)
            return NULL;
        current = current->next;
    }
    return new_head;
}

int exist_variables(struct variable *head, const char *name)
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

void delete_variable(struct variable **head, char *name)
{
    struct variable *p = *head;
    struct variable *prev = NULL;
    while (p != NULL)
    {
        if (strcmp(p->name, name) == 0)
        {
            if (prev == NULL)
                *head = p->next;
            else
                prev->next = p->next;
            free(p->name);
            free(p->value);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void free_variables(struct variable *head)
{
    while (head != NULL)
    {
        struct variable *temp = head;
        head = head->next;
        free(temp->name);
        free(temp->value);
        free(temp);
    }
}
