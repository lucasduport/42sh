#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    // Firsts of shell_command
    if (peek.type == TOKEN_IF || peek.type == TOKEN_WHILE
        || peek.type == TOKEN_UNTIL || peek.type == TOKEN_FOR)
    {
        debug_printf(LOG_PARS,"[PARSER] Shell command - command\n");

        if (parser_shell_command(lex, res) == PARSER_UNEXPECTED_TOKEN)
            return PARSER_UNEXPECTED_TOKEN;
        
        struct ast *tmp_command = *res;
        struct ast *tmp_redir = NULL;

        peek = lexer_peek(lex);
        while (peek.family != TOKEN_FAM_OPERATOR)
        {
            if (parser_redirection(lex, res) == PARSER_UNEXPECTED_TOKEN)
            {
                ast_free(tmp_command);
                ast_free(tmp_redir);
                return PARSER_UNEXPECTED_TOKEN;
            }
            
            ast_add_child_to_child(&tmp_redir, *res);
            peek = lexer_peek(lex);
        }
        if (tmp_redir != NULL)
        {
            ast_add_child_to_child(&tmp_redir, *res);
            *res = tmp_redir;
        }
        return PARSER_OK;
    }
    else
        return parser_simple_command(lex, res);
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res)
{
    debug_printf(LOG_PARS, "[PARSER] In parser simple_command\n");
    struct token peek = lexer_peek(lex);

    struct ast *tmp_redir = NULL;
    struct ast *tmp_assignement = ast_new(AST_ASSIGNMENT);
    struct ast *tmp_command = ast_new(AST_COMMAND);

    // Parse prefix
    while (peek.family == TOKEN_FAM_IO_NUMBER || peek.family == TOKEN_FAM_REDIR || peek.family == TOKEN_FAM_ASSIGNMENT_W)
    {
        enum parser_status stat = 0;
        if (peek.family == TOKEN_FAM_ASSIGNMENT_W)
            stat = parser_prefix(lex, &tmp_assignement);
        else
            stat = parser_prefix(lex, res);
        
        if (stat == PARSER_UNEXPECTED_TOKEN)
            goto error;

        if (peek.family != TOKEN_FAM_ASSIGNMENT_W)
            ast_add_child_to_child(&tmp_redir, *res);
        peek = lexer_peek(lex);
    }
    
    // No word
    peek = lexer_peek(lex);
    if (peek.type != TOKEN_WORD)
    {
        *res = tmp_redir;
        // Minimum one prefix
        if (tmp_redir == NULL)
        {
            token_free(lexer_pop(lex));
            goto error;
        }
        return PARSER_OK;
    }

    // Pop previous word
    peek = lexer_pop(lex);
    tmp_command->arg = list_create(peek.data);

    peek = lexer_peek(lex);
    while (peek.family != TOKEN_FAM_OPERATOR)
    {
        debug_printf(LOG_PARS, "[PARSER] In element loop, peek data = %s\n", peek.data);
        if (peek.family == TOKEN_FAM_IO_NUMBER || peek.family == TOKEN_FAM_REDIR)
        {
            if (parser_element(lex, res) == PARSER_UNEXPECTED_TOKEN)
                goto error;

            ast_add_child_to_child(&tmp_redir, *res);
        }
        else
            parser_element(lex, &tmp_command);
        peek = lexer_peek(lex);
    }

    // Link redir and command
    if (tmp_assignement->arg == NULL)
        ast_free(tmp_assignement);
    else
        ast_add_child_to_child(&tmp_redir, tmp_assignement);
    
    ast_add_child_to_child(&tmp_redir, tmp_command);
    *res = tmp_redir;

    return PARSER_OK;

error:
    ast_free(tmp_redir);
    ast_free(tmp_assignement);
    ast_free(tmp_command);
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_shell_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type == TOKEN_IF)
        return parser_rule_if(lex, res);

    if (peek.type == TOKEN_WHILE)
        return parser_rule_while(lex, res);

    if (peek.type == TOKEN_UNTIL)
        return parser_rule_until(lex, res);

    if (peek.type == TOKEN_FOR)
        return parser_rule_for(lex, res);

    token_free(lexer_pop(lex));
    return PARSER_UNEXPECTED_TOKEN;
}
