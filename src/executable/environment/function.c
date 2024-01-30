#include "environment.h"

int add_function(struct function **head, const char *name, struct ast *body)
{
    struct function *new = calloc(1, sizeof(struct function));
    if (new == NULL)
        return -1;

    new->name = strdup(name);
    new->body = ast_copy(body);
    new->next = *head;
    *head = new;
    return 0;
}

int set_function(struct environment *env, const char *name, struct ast *body)
{
    struct function *current = env->functions;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            ast_free(current->body);
            current->body = ast_copy(body);
            return 0;
        }
        current = current->next;
    }

    // If the function is not found, add it to the list
    return add_function(&env->functions, name, body);
}

struct ast *get_function(struct environment *env, const char *name)
{
    const struct function *current = env->functions;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
            return current->body;
        current = current->next;
    }
    return NULL;
}

struct function *dup_functions(struct function *head)
{
    struct function *new_head = NULL;
    struct function *current = head;
    while (current != NULL)
    {
        add_function(&new_head, current->name, current->body);
        if (new_head == NULL)
            return NULL;
        current = current->next;
    }
    return new_head;
}

int exist_functions(struct function *head, const char *name)
{
    struct function *current = head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
            return 1;
        current = current->next;
    }
    return 0;
}

void delete_function(struct function **head, char *name)
{
    struct function *p = *head;
    struct function *prev = NULL;
    while (p != NULL)
    {
        if (strcmp(p->name, name) == 0)
        {
            if (prev == NULL)
                *head = p->next;
            else
                prev->next = p->next;
            free(p->name);
            ast_free(p->body);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void free_functions(struct function *head)
{
    while (head != NULL)
    {
        struct function *temp = head;
        head = head->next;
        free(temp->name);
        ast_free(temp->body);
        free(temp);
    }
}
