#ifndef BUILTINS_H
#define BUILTINS_H

#include <unistd.h>

struct list
{
    char *current;
    struct list *next;
};

char *list_get_n(struct list *l, size_t n);

/**
 * @brief echo builtin
 * 
 * linked list: echo -> options -> args
 * 
 * @param list linked list
*/
void echo(struct list *list);

#endif /* ! BUILTINS_H */
