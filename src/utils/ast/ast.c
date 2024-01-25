#include "ast.h"

#include <err.h>
#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (!new)
        return NULL;
    new->type = type;
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

struct ast *ast_copy(struct ast *ast)
{
    if (ast == NULL)
        return NULL;

    struct ast *copy = ast_new(ast->type);
    copy->arg = list_copy(ast->arg);
    copy->next = ast_copy(ast->next);
    copy->first_child = ast_copy(ast->first_child);
    return copy;
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
