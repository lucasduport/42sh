#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../../utils/variables/variables.h"

struct environment
{
    struct variable *variables;
};

/**
 * @brief Creates a new environment
 *
 * @return struct environment* The new environment
 */
struct environment *environment_new(void);

/**
 * @brief Dupplicates an environment
 *
 * @param env The environment to copy
 * @return struct environment* The copy of environment `env`
 */
struct environment *dup_environment(struct environment *env);


/**
 * @brief Check if name is the name of environment variable
 * 
 * @param name The variable name
*/
struct environment *check_env_variable(const char *name);


#endif /* ! ENVIRONMENT_H */
