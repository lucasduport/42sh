#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../logger/logger.h"

struct list *list_create(char *current)
{
    struct list *new = calloc(1, sizeof(struct list));
    if (new == NULL)
        return NULL;

    new->current = current;
    return new;
}

void list_append(struct list **l, char *current)
{
    if (*l == NULL)
        *l = list_create(current);
    else
    {
        struct list *p = *l;
        while (p->next != NULL)
            p = p->next;

        p->next = list_create(current);
    }
}

char *list_get_n(struct list *l, size_t n)
{
    while (n != 0 && l != NULL)
    {
        l = l->next;
        n--;
    }

    if (l == NULL)
        return NULL;

    return l->current;
}

struct list *list_copy(struct list *original)
{
    struct list *copy = NULL;
    struct list *p = original;
    while (p != NULL)
    {
        list_append(&copy, strdup(p->current));
        p = p->next;
    }
    return copy;
}

void list_destroy(struct list *l)
{
    while (l != NULL)
    {
        struct list *tmp = l;
        l = l->next;
        free(tmp->current);
        free(tmp);
    }
}
