#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../../utils/list/list.h"
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
 * @brief Free the environment
 * 
 * @param env The environment
*/
void environment_free(struct environment *env);

/**
 * @brief Check if name is the name of environment variable
 *
 * @param name The variable 
 * @return bool
 */
int check_env_variable(const char *name);

/**
 * @brief Check if name is the name of a special var
 * 
 * @param name The variable name
 * @return bool
*/
int check_special_variable(const char *name);

/**
 * @brief Set the $? in the environment
 * 
 * @param env The environment
 * @param return_code The return code of the last command
*/
void set_exit_variale(struct environment *env, int return_code);

/**
 * @brief Set the $* in the environment
 * 
 * @param env The environment
*/
void set_star_variable(struct environment *env);

/**
 * @brief Create the linked list of arguments for the $@ expansion
 * 
 * @param env The environment
 * @return The linked list : "arg1" -> "arg2" -> "arg3" ...
*/
struct list *get_at_variable(struct environment *env);

/**
 * @brief Set the $$ in the environment
 * 
 * @param env The environment
*/
void set_dollar_dollar(struct environment *env);

/**
 * @brief Set the $UID in the environment
 * 
 * @param env The environment
*/
void set_uid(struct environment *env);

/**
 * @brief Set the $RANDOM in the environment
 * 
 * @param env The environment
*/
void set_random(struct environment *env);

/**
 * @brief Set the $1..n in the environment
 * And set $#
 * 
 * @param env The environment
 * @param argc
 * @param argv The list of arguments
*/
void set_number_variable(struct environment *env, int argc, char *argv[]);

/**
 * @brief Set all the special variables in the environment
 * 
 * @param env The environment
 * @param argc
 * @param argv
*/
void set_environment(struct environment *env, int argc, char *argv[]);

#endif /* ! ENVIRONMENT_H */
