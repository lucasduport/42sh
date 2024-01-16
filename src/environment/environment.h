#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../utils/variables/variables.h"

struct environment {
    int fd_in;
    int fd_out;
    int fd_err;

    struct variables **variables;
};

struct environment *environment_new(void);

struct environment *environment_deep_copy(void);

#endif /* ! ENVIRONMENT_H */
