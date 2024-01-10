#ifndef AST_H
#define AST_H

#include <unistd.h>

enum ast_type
{
    AST_IF,
    AST_CONDITION,
    AST_THEN,
    AST_ELSE,
    AST_COMMAND,
    AST_BOOL,
    AST_LIST
};

struct ast
{
    enum ast_type type; ///< AST_TYPE of actual node
    char **content; ///< Argument if there is
    struct ast *next; ///< Point to the next child of the parent's node
    struct ast *first_child; ///< Point to the first child of the node
};

/**
 * @brief Allocate a new ast with the given type
 * 
 * new.type -> type given in argument
 * other attributes -> set to NULL
 *
 * @param parameter1 AST_TYPE of the new ast
 */
struct ast *ast_new(enum ast_type type);

/**
 * @brief Add a new brother at the current node
 * 
 * The new brother is add at the total end of the brother's list
 * 
 * @param parameter1 current ast node
 * @param parameter2 brother that we want to add
*/
void *ast_new_brother(struct ast *ast, struct ast *new_brother)

/**
 * @brief Recursively free the given ast
 *
 * @param parameter1 ast that we want to free
 */
void ast_free(struct ast *ast);

#endif /* !AST_H */