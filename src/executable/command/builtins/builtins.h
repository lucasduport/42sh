#ifndef BUILTINS_H
#define BUILTINS_H

#include <unistd.h>
#include "../utils/list/list.h"

/**
 * @brief echo builtin
 * 
 * @param list Linked list of argument (echo -> ...)
 * @return return code
*/
int builtin_echo(struct list *list);

/**
 * @brief false builtin
 * 
 * @param list Linked list (not used)
  * @return returns 1
*/
int builtin_false(struct list *list);

/**
 * @brief false builtin
 * 
 * @param list Linked list (not used)
  * @return returns 0
*/
int builtin_false(struct list *list);

#endif /* ! BUILTINS_H */
