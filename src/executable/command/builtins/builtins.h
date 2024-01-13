#ifndef BUILTINS_H
#define BUILTINS_H

#include <unistd.h>
#include "../../../utils/list/list.h"
#include "../../../logger/logger.h"

/**
 * @brief echo builtin
 * 
 * @param list Linked list of argument (echo -> ...)
 * @return 0
*/
int builtin_echo(struct list *list);

/**
 * @brief true builtin
 * 
 * @param list Linked list (not used)
 * @return return 0
*/
int builtin_true(struct list *list);

/**
 * @brief false builtin
 * 
 * @param list Linked list (not used)
 * @return return 1
*/
int builtin_false(struct list *list);

#endif /* ! BUILTINS_H */
