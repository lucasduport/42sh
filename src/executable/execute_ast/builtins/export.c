#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* !_POSIX_C_SOURCE */

#include <stdlib.h>

#include "builtins.h"

int builtin_export(struct list *list, struct environment *env)
{
    int code = 0;
    for (struct list *ass = list->next; ass != NULL; ass = ass->next)
    {
        char *variable_name = NULL;
        char *variable_value = NULL;
        if (strchr(ass->current, '=') != NULL)
        {
            variable_name = strtok(ass->current, "=");
            variable_value = strtok(NULL, "=");
            variable_value = expand_string(variable_value, env, &code);

            if (code != 0)
                return set_error(env, STOP, code);

            if (variable_name != NULL && variable_value != NULL)
                setenv(variable_name, variable_value, 1);

            free(variable_value);
        }
        else
        {
            variable_name = ass->current;
            variable_value = get_value(env, variable_name);
            if (variable_name != NULL && variable_value != NULL)
                setenv(variable_name, variable_value, 1);
        }
    }
    return code;
}
