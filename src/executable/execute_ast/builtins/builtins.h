#ifndef BUILTINS_H
#define BUILTINS_H

#include <unistd.h>

#include "../../../logger/logger.h"
#include "../../../utils/list/list.h"
#include "../../environment/environment.h"
#include "../../../utils/variables/variables.h"
#include "../../../expansion/expansion.h"

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

/**
 * @brief exit builtin
 * 
 * @param list Linked list of argument (exit -> ...) 
 * @return int return code 
 */
int builtin_exit(struct list *list, struct environment *env);

/**
 * @brief export builtin
 * 
 * @param list Linked list of argument (export -> ...) 
 * @return int return code 
 */
int builtin_export(struct list *list, struct environment *env);

#endif /* ! BUILTINS_H */
