#include "builtins.h"

int builtin_continue(struct list *list, struct environment *env)
{
    if (list->next != NULL)
    {
        int n = atoi(list->next->current);
        if (n == 0 || list->next->next != NULL)
            return 128;
        env->nb_continue = n;
    }
    else
        env->nb_continue = 1;
    return 0;
}

int builtin_break(struct list *list, struct environment *env)
{
    if (list->next != NULL)
    {
        int n = atoi(list->next->current);
        if (n == 0 || list->next->next != NULL)
            return 128;
        env->nb_break = n;
    }
    else
        env->nb_break = 1;
    return 0;
}
