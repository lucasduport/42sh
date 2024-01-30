#include "ast.h"

static void ast_print_command(struct ast *ast)
{
    if (ast->arg == NULL)
        debug_printf(LOG_AST, "[ empty command ]");
    else
        list_print(ast->arg);
}

static void ast_print_list(struct ast *ast)
{
    struct ast *child = ast->first_child;
    debug_printf(LOG_AST, "list { ");
    while (child != NULL)
    {
        ast_print(child);
        child = child->next;
        if (child != NULL)
            debug_printf(LOG_AST, "; ");
    }
    debug_printf(LOG_AST, " }");
}

static void ast_print_function(struct ast *ast)
{
    debug_printf(LOG_AST, "%s () { ", ast->arg->current);
    ast_print(ast->first_child);
    debug_printf(LOG_AST, "}");
}

static void ast_print_while_until(struct ast *ast)
{
    debug_printf(LOG_AST, "%s { ",
                 (ast->type == AST_WHILE) ? "while" : "until");
    ast_print(ast->first_child);

    struct ast *child = ast->first_child;
    debug_printf(LOG_AST, " } do { ");
    child = child->next;
    ast_print(child);
    debug_printf(LOG_AST, " } done");
}

static void ast_print_for(struct ast *ast)
{
    debug_printf(LOG_AST, "for { ");
    list_print(ast->arg);
    debug_printf(LOG_AST, " } do { ");
    ast_print(ast->first_child);
    debug_printf(LOG_AST, " } done");
}

static void ast_print_if(struct ast *ast)
{
    debug_printf(LOG_AST, "if { ");
    ast_print(ast->first_child);

    struct ast *child = ast->first_child;
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

static void ast_print_neg(struct ast *ast)
{
    debug_printf(LOG_AST, "! ");
    ast_print(ast->first_child);
}

static void ast_print_and_or(struct ast *ast)
{
    debug_printf(LOG_AST, "{ ");
    ast_print(ast->first_child);
    debug_printf(LOG_AST, " } %s { ", (ast->type == AST_AND) ? "&&" : "||");
    ast_print(ast->first_child->next);
    debug_printf(LOG_AST, " }");
}

static void ast_print_pipe(struct ast *ast)
{
    debug_printf(LOG_AST, "{ ");
    ast_print(ast->first_child);
    debug_printf(LOG_AST, " } | { ");
    ast_print(ast->first_child->next);
    debug_printf(LOG_AST, " }");
}

static void ast_print_redir(struct ast *ast)
{
    debug_printf(LOG_AST, "R");
    list_print(ast->arg);
    if (ast->first_child != NULL)
        debug_printf(LOG_AST, "->");
    ast_print(ast->first_child);
}

static void ast_print_assignment(struct ast *ast)
{
    debug_printf(LOG_AST, "A");
    list_print(ast->arg);
    if (ast->first_child != NULL)
        debug_printf(LOG_AST, "->");
    ast_print(ast->first_child);
}

static void ast_print_subshell(struct ast *ast)
{
    debug_printf(LOG_AST, "${ ");
    ast_print(ast->first_child);
    debug_printf(LOG_AST, "}");
}

static void ast_print_case(struct ast *ast)
{
    debug_printf(LOG_AST, "\ncase %s\n", ast->arg->current);
    
    for (struct ast *c = ast->first_child; c != NULL; c = c->next)
        ast_print(c);
    
    debug_printf(LOG_AST, "end_case\n");
}

static void ast_print_item(struct ast *ast)
{
    debug_printf(LOG_AST, "\t");
    list_print(ast->arg);
    debug_printf(LOG_AST, " )\n\t\t");
    ast_print(ast->first_child);
    debug_printf(LOG_AST, "\n\t;;\n");
}

print_ast_node printers[] = { [AST_COMMAND] = ast_print_command,
                              [AST_LIST] = ast_print_list,
                              [AST_FUNC] = ast_print_function,
                              [AST_WHILE] = ast_print_while_until,
                              [AST_UNTIL] = ast_print_while_until,
                              [AST_FOR] = ast_print_for,
                              [AST_IF] = ast_print_if,
                              [AST_NEG] = ast_print_neg,
                              [AST_AND] = ast_print_and_or,
                              [AST_OR] = ast_print_and_or,
                              [AST_PIPE] = ast_print_pipe,
                              [AST_REDIR] = ast_print_redir,
                              [AST_ASSIGNMENT] = ast_print_assignment,
                              [AST_SUBSHELL] = ast_print_subshell,
                              [AST_CASE] = ast_print_case,
                              [AST_ITEM] = ast_print_item};

void ast_print(struct ast *ast)
{
    if (ast == NULL)
        return;
    else
        printers[ast->type](ast);
}
