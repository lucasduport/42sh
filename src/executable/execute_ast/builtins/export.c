#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* !_POSIX_C_SOURCE */

#include <stdlib.h>

#include "builtins.h"

int builtin_export(struct list *list, struct environment *env)
{
    struct list *ass = list->next;
    char *variable_name = NULL;
    char *variable_value = NULL;
    int code = 0;

    if (strchr(ass->current, '=') != NULL)
    {
        variable_name = strtok(ass->current, "=");
        variable_value = expand_string(strtok(NULL, "="), env, &code);
        
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
        if (variable_name != NULL)
            setenv(variable_name, variable_value, 1);
    }

    return code;
}
