#include "builtins.h"

int builtin_exit(struct list *list, struct environment *env)
{
    env->error = STOP;
    int code = 0;
    if (list->next == NULL)
    {
        // If no argument, get return code of last command
        char *val;
        if (env->variables != NULL && (val = get_value(env, "?")) != NULL)
            sscanf(val, "%d", &code);
    }
    else
    {
        if (sscanf(list->next->current, "%d", &code) != 1)
        {
            fprintf(stderr, "exit: numeric argument required\n");
            return set_error(env, STOP, 2);
        }
        if (list->next->next != NULL)
        {
            fprintf(stderr, "exit: too many arguments\n");
            return set_error(env, STOP, 2);
        }
    }
    // Handle negative modulos
    code = (code % 256 + 256) % 256;
    return code;
}
