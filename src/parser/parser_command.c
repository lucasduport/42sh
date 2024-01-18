#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    // TOKEN_WORD -> first of simple_command
    if (peek.type == TOKEN_WORD)
        return parser_simple_command(lex, res);

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
        while (peek.family != TOKEN_FAM_OPERATOR && peek.type != TOKEN_EOF)
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

    token_free(lexer_pop(lex));
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res)
{
    debug_printf(LOG_PARS, "[PARSER] In parser simple_command\n");
    struct token peek = lexer_peek(lex);
    struct ast *tmp_redir = NULL;

    // Parse prefix
    while (peek.family == TOKEN_FAM_IO_NUMBER || peek.family == TOKEN_FAM_REDIR)
    {
        debug_printf(LOG_PARS, "[PARSER] In prefix loop\n");
        if (parser_prefix(lex, res) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_redir);
            return PARSER_UNEXPECTED_TOKEN;
        }
        ast_add_child_to_child(&tmp_redir, *res);
        peek = lexer_peek(lex);
    }
    
    // No word
    peek = lexer_peek(lex);
    if (peek.type != TOKEN_WORD)
    {
        *res = tmp_redir;
        // Minimum one prefix
        if (tmp_redir != NULL)
            return PARSER_OK;
        
        token_free(lexer_pop(lex));
        return PARSER_UNEXPECTED_TOKEN;
    }

    // Pop previous word
    peek = lexer_pop(lex);
    struct ast *tmp_command = ast_new(AST_COMMAND);
    tmp_command->arg = list_create(peek.data);

    peek = lexer_peek(lex);
    while (peek.family != TOKEN_FAM_OPERATOR && peek.type != TOKEN_EOF)
    {
        debug_printf(LOG_PARS, "[PARSER] In element loop, peek data = %s\n", peek.data);
        if (peek.family == TOKEN_FAM_IO_NUMBER || peek.family == TOKEN_FAM_REDIR)
        {
            debug_printf(LOG_PARS, "[PARSER] In element loop, IO_number\n");
            if (parser_element(lex, res) == PARSER_UNEXPECTED_TOKEN)
            {
                ast_free(tmp_command);
                ast_free(tmp_redir);
                return PARSER_UNEXPECTED_TOKEN;
            }
            ast_add_child_to_child(&tmp_redir, *res);
        }
        else
            parser_element(lex, &tmp_command);
        peek = lexer_peek(lex);
    }

    // Link redir and command
    ast_add_child_to_child(&tmp_redir, tmp_command);
    *res = tmp_redir;

    return PARSER_OK;
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
