#include "ast.h"

#include <err.h>
#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = malloc(sizeof(struct ast));
    if (!new)
        return NULL;
    new->type = type;
    new->arg = NULL;
    new->first_child = NULL;
    new->next = NULL;
    return new;
}

void ast_add_brother(struct ast *ast, struct ast *new_brother)
{
    struct ast *p = ast;
    while (p->next != NULL)
        p = p->next;
    p->next = new_brother;
}

void ast_add_child_to_child(struct ast **ast, struct ast *new_child)
{
    if (*ast == NULL)
        *ast = new_child;
    else
    {
        struct ast *p = *ast;
        while (p->first_child != NULL)
            p = p->first_child;
        p->first_child = new_child;
    }
}

void ast_free(struct ast *ast)
{
    if (ast != NULL)
    {
        // Free first child of the ast
        ast_free(ast->first_child);

        // Free argument of the node
        list_destroy(ast->arg);

        // Free next child of the same parent
        ast_free(ast->next);

        free(ast);
    }
}
