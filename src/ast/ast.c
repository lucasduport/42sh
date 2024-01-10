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

void *ast_add_brother(struct ast *ast, struct ast *new_brother)
{
    struct ast *p = ast;
    while (p->next != NULL)
        p = p->next;
    p->next = new_brother;
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
        return;

    //Free other child of the same parent
    struct ast *p = ast->next;
    struct ast *tmp;
    while (p != NULL)
    {
        tmp = p;
        p = p->next;
        ast_free(tmp);
    }

    //Free first child of the ast
    ast_free(ast->first_child);

    free(ast);
}