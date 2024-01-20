#ifndef EXPANSION_H
#define EXPANSION_H

#include "../executable/environment/environment.h"
#include "../utils/list/list.h"
#include "../utils/variables/variables.h"

/**
 * @brief Try every expansions on a string
 * 
 * @param str String to expand
 * @param env Current environment
 * @param ret Pointer on return value
 * 
 * @return copy of arguments list, expand
*/
char *expand_string(char *str, struct environment *env, int *ret);

/**
 * @brief Try every expansions on the arguments
 *
 * @param arguments Arguments to expand
 * @param env Current environment
 * 
 * @return copy of arguments list expand
 */
struct list *expansion(struct list *list, struct environment *env, int *ret);

#endif /* ! EXPANSION_H */
