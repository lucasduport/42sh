#include "builtins.h"

int builtin_true(struct list *list)
{
    if (list == NULL)
        debug_printf(LOG_EXEC, "[EXECUTE] Missing bool argument\n");
    return 0;
}

int builtin_false(struct list *list)
{
    if (list == NULL)
        debug_printf(LOG_EXEC, "[EXECUTE] Missing bool argument\n");
    return 1;
}
