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

void ast_print(struct ast *ast)
{
    if (ast == NULL)
        return;

    
    if (ast->type == AST_IF)
    {
        printf("if { ");
        if (ast->first_child == NULL)
        {
            printf("AST error - 'if' node - no condition\n");
            return;
        }
        ast_print(ast->first_child);

        struct ast *child = ast->first_child;
        if (child->next == NULL)
        {
            printf("AST error - 'if' node - no then\n");
            return;
        }
        printf(" } then { ");
        child = child->next;
        ast_print(child);

        if (child->next != NULL)
        {
            printf(" } else { ");
            ast_print(child->next);
            printf(" }\n");
        }
    }
    
    else if (ast->type == AST_COMMAND)
    {
        if (ast->arg == NULL)
            printf("[ empty command ]");

        else
            list_print(ast->arg);
    }

    else
    {
        struct ast *child = ast->first_child;
        printf("list { ");
        while (child != NULL)
        {
            ast_print(child);
            child = child->next;
        }
        printf(" }");
    }
}
