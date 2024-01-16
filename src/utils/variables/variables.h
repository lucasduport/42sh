#ifndef VARIABLES_H
#define VARIABLES_H

#include "../../logger/logger.h"

struct variable
{
    char *name;
    char *value;
    struct variable *next;
};

#endif /* ! VARIABLES_H */
