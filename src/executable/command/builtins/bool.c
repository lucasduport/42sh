#include "builtins.h"

int builtin_true(struct list *list)
{
    if (list != NULL)
        debug_printf("bool.c: a boolean should not take any argument");
    return 0;
}

int builtin_false(struct list *list)
{
    if (list != NULL)
        debug_printf("bool.c: a boolean should not take any argument");
    return 1;
}