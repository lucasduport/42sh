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
int builtin_true(struct list *list, struct environment *env);

/**
 * @file bool.c
 * @brief false builtin
 *
 * @param list Linked list (not used)
 * @return return 1
 */
int builtin_false(struct list *list, struct environment *env);

/**
 * @file cd.c
 * @brief cd builtin
 *
 * @param list Linked list (cd -> ...)
 * @return 0
 */
int builtin_cd(struct list *list, struct environment *env);

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
int builtin_echo(struct list *list, struct environment *env);

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
 * @param list Linked list of argument (continue -> ...)
 * @return int return code
 */
int builtin_continue(struct list *list, struct environment *env);

/**
 * @file stop_loop.c
 * @brief break builtin
 *
 * @param list Linked list of argument (break -> ...)
 * @return int return code
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

/**
 * @file alias.c
 * @brief alias builtin
 *
 * @param list Linked list of argument (alias -> ...)
 * @return int return code
 */
int builtin_alias(struct list *list, struct environment *env);

/**
 * @file alias.c
 * @brief unalias builtin
 *
 * @param list Linked list of argument (unalias -> ...)
 * @return int return code
 */
int builtin_unalias(struct list *arguments, struct environment *env);

/**
 * @file alias.c
 * @brief set the new aliases after execution
 *
 * @param env environment
 * @return void
 */
void update_aliases(struct environment *env);

/**
 * @file alias.c
 * @brief expands the alias in the string (in place) if needed
 *
 * @param alias aliases to try to expand
 * @param string string to expand
 * @return void
 */
void alias_expansion(struct variable *alias, char **string);

#endif /* ! BUILTINS_H */
