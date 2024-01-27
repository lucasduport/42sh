#include "builtins.h"

int builtin_true(__attribute__((unused)) struct list *list, 
                __attribute__((unused)) struct environment *env)
{
    return 0;
}

int builtin_false(__attribute__((unused)) struct list *list, 
                __attribute__((unused)) struct environment *env)
{
    return 1;
}
