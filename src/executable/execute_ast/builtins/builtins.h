#ifndef BUILTINS_H
#define BUILTINS_H

#include <unistd.h>

#include "../../../expansion/expansion.h"
#include "../../../logger/logger.h"
#include "../../../utils/list/list.h"
#include "../../environment/environment.h"

/**
 * @file echo.c
 * @brief echo builtin
 *
 * @param list Linked list of argument (echo -> ...)
 * @return 0
 */
int builtin_echo(struct list *list);

/**
 * @file bool.c
 * @brief true builtin
 *
 * @param list Linked list (not used)
 * @return return 0
 */
int builtin_true(struct list *list);

/**
 * @file bool.c
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

/**
 * @file cd.c
 * @brief cd builtin
 * 
 * @param list Linked list (cd -> ...)
 * @return 0
*/
int builtin_cd(struct list *list);

#endif /* ! BUILTINS_H */
