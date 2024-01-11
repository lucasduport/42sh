#include <stdlib.h>
#include <stdio.h>

#include "../../logger/logger.h"
#include "list.h"

struct list *list_create(char *current)
{
    struct list *new = calloc(1, sizeof(struct list));
    if (new == NULL)
    {
        debug_printf("list_create: struct allocation failed\n");
        return NULL;
    }

    new->current = current;
    return new;
}

void list_append(struct list *l, char *current)
{   
    while (l->next != NULL)
        l = l->next;
    
    l->next = list_create(current);
}

char *list_get_n(struct list *l, size_t n)
{
    while (n != 0 && l != NULL)
    {
        l = l->next;
        n--;
    }

    if (l == NULL)
    {
        debug_printf("list_get_n: index out of range\n");
        return NULL;
    }

    return l->current;
}

void list_destroy(struct list *l)
{
    while (l != NULL)
    {
        struct list *tmp = l;
        l = l->next;
        free(tmp);
    }
}

void list_print(struct list *l)
{
    printf("[ ");

    while (l->next != NULL)
    {
        printf("%s, ", l->current);
    }

    printf("%s ]", l->current);
}
