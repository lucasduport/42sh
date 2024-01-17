#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../../utils/variables/variables.h"

struct environment
{
    int fd_in;
    int fd_out;
    int fd_err;

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

#endif /* ! ENVIRONMENT_H */
