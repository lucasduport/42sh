#ifndef EXPANSION_H
#define EXPANSION_H

#include "../executable/environment/environment.h"
#include "../utils/list/list.h"
#include "../utils/variables/variables.h"

/**
 * @brief Try every expansions on the arguments
 *
 * @param arguments Arguments to expand
 * @param env Environment
 * 
 * @return copy of arguments list expand
 */
struct list *expansion(struct list *list, struct environment *env, int *ret);

#endif /* ! EXPANSION_H */
