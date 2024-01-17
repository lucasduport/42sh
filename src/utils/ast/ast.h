#ifndef AST_H
#define AST_H

#include <unistd.h>

#include "../../logger/logger.h"
#include "../list/list.h"

enum ast_type
{
    AST_IF,
    AST_WHILE,
    AST_UNTIL,
    AST_COMMAND,
    AST_LIST,
    AST_AND,
    AST_OR,
    AST_NEG,
    AST_PIPE,
    AST_ASSIGNMENT
};

struct ast
{
    enum ast_type type; ///< AST_TYPE of actual node
    struct list *arg; ///< List of arguments if there are some
    struct ast *next; ///< Point to the next child of the parent's node
    struct ast *first_child; ///< Point to the first child of the node
};

/**
 * @brief Create a new ast with the given type
 *
 * new.type -> type given in argument
 * other attributes -> set to NULL
 *
 * @param parameter1 AST_TYPE of the new ast
 *
 * @return ast structure initialize with the correct type
 */
struct ast *ast_new(enum ast_type type);

/**
 * @brief Add a new brother at the current node.
 *
 * The new brother is add at the total end of the brother's list.
 *
 * @param parameter1 current ast node
 * @param parameter2 brother that we want to add
 */
void ast_add_brother(struct ast *ast, struct ast *new_brother);

/**
 * @brief Recursively free the given ast
 *
 * @param parameter1 ast that we want to free
 */
void ast_free(struct ast *ast);

/**
 * @brief Print ast format pretty-print
 *
 * @param parameter1 ast that we want to print
 */
void ast_print(struct ast *ast);

#endif /* !AST_H */
