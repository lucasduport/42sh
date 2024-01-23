#ifndef VARIABLES_H
#define VARIABLES_H

#include "../../executable/environment/environment.h"
#include "../../logger/logger.h"

struct variable
{
    char *name;
    char *value;
    struct variable *next;
};

/**
 * @brief Set the value of a variable, or add it to the list if it doesn't exist
 *
 * @param head The head of the list
 * @param name The name of the variable
 * @param value The value of the variable
 * @return int 0 on success, -1 on error
 */
int set_variable(struct variable **head, const char *name, char *value);

/**
 * @brief Get the value of a variable
 *
 * @param head The head of the list
 * @param name The name of the variable
 * @return char* The value of the variable
 */
char *get_value(const struct variable *head, const char *name);

/**
 * @brief Duplicate a list of variables
 *
 * @param head The head of the list
 * @return struct variable* The new head of the list
 */
struct variable *dup_variables(struct variable *head);

/**
 * @brief Free a list of variables
 *
 * @param head The head of the list
 */
void free_variables(struct variable *head);

/**
 * @brief Debug function that prints the var list
 *
 * @param head The head of the list
 */
void print_variables(struct variable *head);

#endif /* ! VARIABLES_H */
