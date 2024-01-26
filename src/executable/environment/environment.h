#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../../logger/logger.h"
#include "../../utils/list/list.h"
#include "../../utils/ast/ast.h"

#define stop FILE_NOT_FOUND
/**
 * Don't forget to change the stop macro
 * < don't stop, >= stop
*/
enum type_error
{
    NO_ERROR,
    WRONG_COMMAND,
    // Stop after
    FILE_NOT_FOUND,
    UNSETABLE_VARIABLE,
    FAILED_EXPAND,
    EXIT_BUILT,
    OURSELF
};

struct environment
{
    struct variable *variables;
    struct function *functions;
    int nb_break;
    int nb_continue;
    int nb_loop;
    bool is_command;
    enum type_error error;
};


/**
 * @file environment.c
 * @brief Creates a new environment
 *
 * @return struct environment* The new environment
 */
struct environment *environment_new(void);

/**
 * @file environment.c
 * @brief Dupplicates an environment
 *
 * @param env The environment to copy
 * @return struct environment* The copy of environment `env`
 */
struct environment *dup_environment(struct environment *env);

/**
 * @file environment.c
 * @brief Free the environment
 *
 * @param env The environment
 */
void environment_free(struct environment *env);

/**
 * @file environment.c
 * @param name The variable
 * 
 * @return bool
 */
int check_env_variable(const char *name);

/**
 * @file environment.c
 * @brief Set all the special variables in the environment
 *
 * @param env The environment
 * @param argc
 * @param argv
 */
void set_environment(struct environment *env, int argc, char *argv[]);

/**
 * @file environment.c
 * @brief Set error attribut of environment
 * 
 * @return ret_code
*/
int set_error_value(struct environment *env, enum type_error type, int ret_code);

struct function
{
    char *name;
    struct ast *body;
    struct function *next;
};

/**
 * @file function.c
 * @brief Add a new function to the list
 * 
 * @param head Head of function's list
 * @param name Name of the function
 * @param body AST body of function
*/
int add_function(struct function **head, const char *name, struct ast *body);

/**
 * @file function.c
 * @brief Set function int the environment and if it's not exist, add it
 * 
 * @param env Current environment
 * @param name Name of the function
 * @param body AST body of function
*/
int set_function(struct environment *env, const char *name, struct ast *body);

/**
 * @file function.c
 * @brief Get back function, given its name
 * 
 * @param env Current environment
 * @param name Name of the function
 * 
 * @return NULL if the name was not found (don't return a copy)
*/
struct ast *get_function(struct environment *env, const char *name);

/**
 * @file function.c
 * @brief Duplicate a list of functions, including name and ast
*/
struct function *dup_functions(struct function *head);

void free_functions(struct function *head);


struct variable
{
    char *name;
    char *value;
    struct variable *next;
};

/**
 * @file special_variable.c
 * 
 * @brief Set the $1..n in the environment
 * And set $#
 *
 * @param env The environment
 * @param argc
 * @param argv The list of arguments
 */
void set_number_variable(struct environment *env, int argc, char *argv[]);

/**
 * @file special_variable.c
 * @brief Set the $? in the environment
 *
 * @param env The environment
 * @param return_code The return code of the last command
 */
void set_exit_variable(struct environment *env, int return_code);

/**
 * @file special_variable.c
 * @brief Set the star variable object
 *
 * @param env The environment
 */
void set_star_variable(struct environment *env);

/**
 * @file special_variable.c
 * @brief Create the linked list of arguments for the $@ expansion
 *
 * @param env The environment
 * @return The linked list : "arg1" -> "arg2" -> "arg3" ...
 */
struct list *get_at_variable(struct environment *env);

/**
 * @file special_variable.c
 * @brief Set the $$ in the environment
 *
 * @param env The environment
 */
void set_dollar_dollar(struct environment *env);

/**
 * @file special_variable.c
 * @brief Set the $UID in the environment
 *
 * @param env The environment
 */
void set_uid(struct environment *env);

/**
 * @file special_variable.c
 * @brief Set the $RANDOM in the environment
 *
 * @param env The environment
 */
void set_random(struct environment *env);

/**
 * @file variable.c
 * @brief Add a new variable to the list
 *
 * @param head The head of the list
 * @param name The name of the variable
 * @param value The value of the variable
 * @return int 0 on success, -1 on error
 */
int add_variable(struct variable **head, const char *name, char *value);

/**
 * @file variable.c
 * @brief Set the value of a variable, or add it to the list if it doesn't exist
 *
 * @param env The environment to add the variable to
 * @param name The name of the variable
 * @param value The value of the variable
 * 
 * @return int 0 on success, -1 on error
 */
int set_variable(struct environment *env, const char *name, char *value);

/**
 * @file variable.c
 * @brief Get the value of a variable
 *
 * @param head The head of the list
 * @param name The name of the variable
 * @return char* The value of the variable
 */
char *get_value(struct environment *env, const char *name);

/**
 * @file variable.c
 * @brief Duplicate a list of variables
 *
 * @param head The head of the list
 * @return struct variable* The new head of the list
 */
struct variable *dup_variables(struct variable *head);

/**
 * @file variable.c
 * @brief Free a list of variables
 *
 * @param head The head of the list
 */
void free_variables(struct variable *head);

/**
 * @file variable.c
 * @brief Debug function that prints the var list
 *
 * @param head The head of the list
 */
void print_variables(struct variable *head);

#endif /* ! ENVIRONMENT_H */
