#include "builtins.h"

int builtin_continue(struct list *arg, struct environment *env)
{
    if (arg->next != NULL)
    {
        int n = atoi(arg->next->current);
        if (n == 0 || arg->next->next != NULL)
            return 128;
        env->nb_continue = n;
    }
    else
        env->nb_continue = 1;
    return 0;
}

int builtin_break(struct list *arg, struct environment *env)
{
    if (arg->next != NULL)
    {
        int n = atoi(arg->next->current);
        if (n == 0 || arg->next->next != NULL)
            return 128;
        env->nb_break = n;
    }
    else
        env->nb_break = 1;
    return 0;
}
