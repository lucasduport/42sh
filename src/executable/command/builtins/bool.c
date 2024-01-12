#include "builtins.h"

int builtin_true(struct list *list)
{
    if (list == NULL)
        debug_printf("[EXECUTE] Missing bool argument");
    return 0;
}

int builtin_false(struct list *list)
{
    if (list == NULL)
        debug_printf("[EXECUTE] Missing bool argument");
    return 1;
}