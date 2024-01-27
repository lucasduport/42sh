#include "stack.h"

#include <stdlib.h>

void stack_push(struct stack **s, enum quote_type q)
{
    struct stack *elt = malloc(sizeof(struct stack));
    elt->data = q;
    elt->next = *s;
    *s = elt;
}

enum quote_type stack_pop(struct stack **s)
{
    if (s == NULL || *s == NULL)
        return EMPTY;
    struct stack *first = *s;
    enum quote_type res = first->data;
    struct stack *nxt = first->next;
    free(first);
    *s = nxt;
    return res;
}

enum quote_type stack_peek(struct stack *s)
{
    if (!s)
        return EMPTY;
    return s->data;
}

int stack_destroy(struct stack *s)
{
    if (s == NULL)
        return 1;
    for (; s != NULL;)
    {
        struct stack *nxt = s->next;
        free(s);
        s = nxt;
    }
    return 0;
}

int is_empty(struct stack *s)
{
    return s == NULL;
}
