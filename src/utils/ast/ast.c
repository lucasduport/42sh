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

void ast_print(struct ast *ast)
{
    if (ast == NULL)
        return;

    if (ast->type == AST_IF)
    {
        debug_printf(LOG_AST, "if { ");
        if (ast->first_child == NULL)
        {
            debug_printf(LOG_AST, "AST error - 'if' node - no condition\n");
            return;
        }
        ast_print(ast->first_child);

        struct ast *child = ast->first_child;
        if (child->next == NULL)
        {
            debug_printf(LOG_AST, "AST error - 'if' node - no then\n");
            return;
        }
        debug_printf(LOG_AST, " } then { ");
        child = child->next;
        ast_print(child);

        if (child->next != NULL)
        {
            debug_printf(LOG_AST, " } else { ");
            ast_print(child->next);
            debug_printf(LOG_AST, " }");
        }
    }

    else if (ast->type == AST_WHILE || ast->type == AST_UNTIL)
    {
        debug_printf(LOG_AST, "%s { ",
                     (ast->type == AST_WHILE) ? "while" : "until");
        if (ast->first_child == NULL)
        {
            debug_printf(LOG_AST, "AST error - 'while' or 'until' node - no condition\n");
            return;
        }
        ast_print(ast->first_child);

        struct ast *child = ast->first_child;
        if (child->next == NULL)
        {
            debug_printf(LOG_AST, "AST error - 'while' node - no do\n");
            return;
        }
        debug_printf(LOG_AST, " } do { ");
        child = child->next;
        ast_print(child);
        debug_printf(LOG_AST, " } done");
    }

    else if (ast->type == AST_AND || ast->type == AST_OR)
    {
        if (ast->first_child == NULL || ast->first_child->next == NULL)
        {
            debug_printf(LOG_AST, "AST error - 'and' or 'or' node - missing command\n");
            return;
        }
        debug_printf(LOG_AST, "{ ");
        ast_print(ast->first_child);
        debug_printf(LOG_AST, " } %s { ", (ast->type == AST_AND) ? "&&" : "||");
        ast_print(ast->first_child->next);
        debug_printf(LOG_AST, " }");
    }

    else if (ast->type == AST_PIPE)
    {
        if (ast->first_child == NULL || ast->first_child->next == NULL)
        {
            debug_printf(LOG_AST, "AST error - 'and' or 'or' node - missing command\n");
            return;
        }
        debug_printf(LOG_AST, "{ ");
        ast_print(ast->first_child);
        debug_printf(LOG_AST, " } | { ");
        ast_print(ast->first_child->next);
        debug_printf(LOG_AST, " }");
    }

    else if (ast->type == AST_COMMAND)
    {
        if (ast->arg == NULL)
            debug_printf(LOG_AST, "[ empty command ]");

        else
            list_print(ast->arg);
    }

    else
    {
        struct ast *child = ast->first_child;
        debug_printf(LOG_AST, "list { ");
        while (child != NULL)
        {
            ast_print(child);
            child = child->next;
        }
        debug_printf(LOG_AST, " }");
    }
}
