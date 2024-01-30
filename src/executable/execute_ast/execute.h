#ifndef EXECUTE_H
#define EXECUTE_H

#include "../../expansion/expansion.h"
#include "../../logger/logger.h"
#include "../../utils/ast/ast.h"
#include "../../utils/list/list.h"
#include "../environment/environment.h"
#include "builtins/builtins.h"

/** ERROR VALUE
 *  0    if successfull
 *  127  if there is wrong command
 *  128  if there is wrong argument given to break or continue
 *  2    expansion error
 * -1    if there is an error on our part
 *  1    otherwise
 */

typedef int (*exec_ast_node)(struct ast *ast, struct environment *env);
typedef int (*exec_builtins)(struct list *list, struct environment *env);

/**
 * @file execute_commande.c
 * @brief Execute command node
 *
 * @param ast 'command' node
 * @return return value from execution of the command
 */
int execute_command(struct ast *ast, struct environment *env);

/**
 * @file execute_command.c
 * @brief Execute subshell node (command in parenthesis)
 *
 * @param ast 'subshell' node
 * @param env environment
 * @return int return value from execution of the command
 */
int execute_subshell(struct ast *ast, struct environment *env);

/**
 * @file execute_command.c
 * @brief Set function (used when AST_FUNC is found)
 *
 * @return -1 if failed; 0 otherwise
 */
int execute_function(struct ast *ast, struct environment *env);

/**
 * @file execute_condition.c
 * @brief Execute if node
 *
 * @param ast 'if' node
 * @return value of 'then' execution or 'else' execution or 0 by default
 */
int execute_if(struct ast *ast, struct environment *env);

/**
 * @file execute_condition.c
 * @brief Execute 'and' node
 *
 * @param ast Current ast
 * @return return value from execution
 */
int execute_and(struct ast *ast, struct environment *env);

/**
 * @file execute_condition.c
 * @brief Execute 'or' node
 *
 * @param ast Current ast
 * @return return value from execution
 */
int execute_or(struct ast *ast, struct environment *env);

/**
 * @brief Execute 'or' node
 * @file execute_condition.c
 *
 * @param ast Current ast
 * @return return value from execution
 */
int execute_neg(struct ast *ast, struct environment *env);

/**
 * @brief Execute 'case' node
 *
 * @return 0 if no match, result of execution otherwise
 */
int execute_case(struct ast *ast, struct environment *env);

/**
 * @file execute_loop.c
 * @brief Execute while node
 *
 * @param ast 'while' node
 * @return return value from execution of last command, 0 if nothing is execute
 */
int execute_while(struct ast *ast, struct environment *env);

/**
 * @file execute_loop.c
 * @brief Execute until node
 *
 * @param ast 'until' node
 * @return return value from execution of last command, 0 if nothing is execute
 */
int execute_until(struct ast *ast, struct environment *env);

/**
 * @file execute_loop.c
 * @brief Execute for node
 *
 * @param ast 'for' node
 * @return return value from execution of last command, 0 if nothing is execute
 */
int execute_for(struct ast *ast, struct environment *env);

struct redirection
{
    int word_fd;
    int io_number;
};

/**
 * @file execute_redir.c
 * @brief Execute general ast
 *
 * @param ast 'pipe' node
 * @return value of the execution
 */
int execute_pipe(struct ast *ast, struct environment *env);

/**
 * @file execute_redir.c
 * @brief Execute redirection for redirection.
 *
 * @param ast 'redir' node
 * @return value of the execution
 */
int execute_redir(struct ast *ast, struct environment *env);

/**
 * @file execute.c
 * @brief Execute the assignment node
 *
 * @param ast 'assignment' node"
 * @return execution value of the assignment
 */
int execute_assignment(struct ast *ast, struct environment *env);

/**
 * @file execute.c
 * @brief Execute list node
 *
 * @param ast 'list' node
 * @return return value from execution of last command
 */
int execute_list(struct ast *ast, struct environment *env);

/**
 * @file execute.c
 * @brief Execute general ast
 *
 * @param ast AST that we want to execute
 * @return value of the execution
 */
int execute_ast(struct ast *ast, struct environment *env);

#endif /* ! EXECUTE_H */
