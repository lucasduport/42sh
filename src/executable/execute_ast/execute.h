#ifndef EXECUTE_H
#define EXECUTE_H

#include "../../logger/logger.h"
#include "../../utils/ast/ast.h"
#include "../../utils/list/list.h"
#include "../command/builtins/builtins.h"
#include "../environment/environment.h"

/** ERROR VALUE
 *  0    if successfull
 *  127  if there is wrong command
 * -1    if there is an error on our part
 *  1    otherwhise
 * */

/**
 * @brief Execute general ast
 *
 * @param ast AST that we want to execute
 * @return value of the execution
 */
int execute_ast(struct ast *ast, struct environment *env);

/**
 * @brief Execute general ast
 *
 * @param ast AST that we want to execute
 * @return value of the execution
 */
int execute_pipe(struct ast *ast, struct environment *env);

#endif /* ! EXECUTE_H */
