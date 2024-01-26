#ifndef BUILTINS_H
#define BUILTINS_H

#include <unistd.h>

#include "../../../expansion/expansion.h"
#include "../../../logger/logger.h"
#include "../../../utils/list/list.h"
#include "../../environment/environment.h"

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
 * @file dot.c
 * @brief dot builtin as described in the SCL
 *
 * @param list Linked list of argument (dot -> ...)
 * @return int return code
 *
 */
int builtin_dot(struct list *list, struct environment *env);

/**
 * @file echo.c
 * @brief echo builtin
 *
 * @param list Linked list of argument (echo -> ...)
 * @return 0
 */
int builtin_echo(struct list *list);

/**
 * @file exit.c
 * @brief exit builtin
 *
 * @param list Linked list of argument (exit -> ...)
 * @return int return code
 */
int builtin_exit(struct list *list, struct environment *env);

/**
 * @file export.c
 * @brief export builtin
 *
 * @param list Linked list of argument (export -> ...)
 * @return int return code
 */
int builtin_export(struct list *list, struct environment *env);

/**
 * @file stop_loop.c
 * @brief continue builtin
 *
 */
int builtin_continue(struct list *list, struct environment *env);

/**
 * @file stop_loop.c
 * @brief exec 'break' builtin
 */
int builtin_break(struct list *list, struct environment *env);

/**
 * @file unset.c
 * @brief unset builtin
 *
 * @param list Linked list of argument (unset -> ...)
 * @return int return code
 */
int builtin_unset(struct list *list, struct environment *env);

#endif /* ! BUILTINS_H */
