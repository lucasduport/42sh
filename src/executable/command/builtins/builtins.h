#ifndef BUILTINS_H
#define BUILTINS_H

#include <unistd.h>
#include "../utils/list/list.h"

/**
 * @brief echo builtin
 * 
 * linked list: echo -> options -> args
 * 
 * @param list linked list
*/
void echo(struct list *list);

#endif /* ! BUILTINS_H */
