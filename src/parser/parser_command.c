#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    // Firsts of shell_command
    if (peek.type == TOKEN_IF || peek.type == TOKEN_WHILE
        || peek.type == TOKEN_UNTIL || peek.type == TOKEN_FOR)
    {
        debug_printf(LOG_PARS, "[PARSER] Shell command - command\n");

        if (parser_shell_command(lex, res) == PARSER_ERROR)
            return PARSER_ERROR;

        struct ast *tmp_command = *res;
        struct ast *tmp_redir = NULL;

        peek = lexer_peek(lex);
        while (peek.family != TOKEN_FAM_OPERATOR)
        {
            if (parser_redirection(lex, res) == PARSER_ERROR)
            {
                ast_free(tmp_command);
                ast_free(tmp_redir);
                return PARSER_ERROR;
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
    {
        if (peek.type == TOKEN_WORD && peek.family != TOKEN_FAM_ASSW)
        {
            struct token word = peek;
            lexer_pop(lex);
            peek = lexer_peek(lex);
            if (peek.type == TOKEN_LEFT_PAR)
                return parser_fundec(lex, res, &word);
            return parser_simple_command(lex, res, &word);
        }
        return parser_simple_command(lex, res, NULL);
    }
}

static enum parser_status errorsc(struct ast *tmp_redir,
                                  struct ast *tmp_assignment,
                                  struct ast *tmp_command)
{
    ast_free(tmp_redir);
    ast_free(tmp_assignment);
    ast_free(tmp_command);
    return PARSER_ERROR;
}

static void link_redir_command(struct ast **res, struct ast **tmp_redir,
                               struct ast **tmp_assignment,
                               struct ast **tmp_command)
{
    if ((*tmp_assignment)->arg == NULL)
        ast_free(*tmp_assignment);
    else
        ast_add_child_to_child(tmp_redir, *tmp_assignment);

    ast_add_child_to_child(tmp_redir, *tmp_command);
    *res = *tmp_redir;
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res, struct token *w)
{
    struct token peek = lexer_peek(lex); 
    struct ast *tmp_redir = NULL;
    struct ast *tmp_assignment = ast_new(AST_ASSIGNMENT);
    struct ast *tmp_command = ast_new(AST_COMMAND);

    if (w != NULL)
        tmp_command->arg = list_create(w->data);
    else
    {
        while (peek.family == TOKEN_FAM_IO_NUMBER || peek.family == TOKEN_FAM_REDIR
            || peek.family == TOKEN_FAM_ASSW)
        {
            if (parser_prefix(lex, (peek.family == TOKEN_FAM_ASSW) ? &tmp_assignment : res) == PARSER_ERROR)
                return errorsc(tmp_redir, tmp_assignment, tmp_command);

            if (peek.family != TOKEN_FAM_ASSW)
                ast_add_child_to_child(&tmp_redir, *res);
            peek = lexer_peek(lex);
        }
    }

    // No word
    peek = lexer_peek(lex);
    if (w == NULL && peek.type != TOKEN_WORD)
    {
        *res = tmp_redir;
        // Minimum one prefix
        if (tmp_redir == NULL && tmp_assignment->arg == NULL)
        {
            token_free(lexer_pop(lex));
            return errorsc(tmp_redir, tmp_assignment, tmp_command);
        }
        ast_add_child_to_child(&tmp_redir, tmp_assignment);
        ast_free(tmp_command);
        *res = tmp_redir;
        return PARSER_OK;
    }

    // Pop previous word
    if (w == NULL)
    {
        peek = lexer_pop(lex);
        tmp_command->arg = list_create(peek.data);
    }

    peek = lexer_peek(lex);
    while (peek.family != TOKEN_FAM_OPERATOR)
    {
        if (peek.family == TOKEN_FAM_IO_NUMBER
            || peek.family == TOKEN_FAM_REDIR)
        {
            if (parser_element(lex, res) == PARSER_ERROR)
                return errorsc(tmp_redir, tmp_assignment, tmp_command);

            ast_add_child_to_child(&tmp_redir, *res);
        }
        else
            parser_element(lex, &tmp_command);
        peek = lexer_peek(lex);
    }

    // Link redir and command
    link_redir_command(res, &tmp_redir, &tmp_assignment, &tmp_command);
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
    return PARSER_ERROR;
}

enum parser_status parser_fundec(struct lexer *lex, struct ast **res, struct token *w)
{
    struct ast *tmp_res = ast_new(AST_FUNC);
    tmp_res->arg = list_create(w->data);
    
    struct token peek = lexer_peek(lex);
    token_free(lexer_pop(lex));
    if (peek.type != TOKEN_LEFT_PAR)
        goto error;
    
    peek = lexer_peek(lex);
    token_free(lexer_pop(lex));
    if (peek.type != TOKEN_RIGHT_PAR)
        goto error;
    
    skip_newline(lex);
    enum parser_status code = parser_shell_command(lex, res);
    if (code == PARSER_ERROR)
        goto error;
    tmp_res->first_child = *res;
    *res = tmp_res;

error:
    ast_free(tmp_res);
    return PARSER_ERROR;
}
