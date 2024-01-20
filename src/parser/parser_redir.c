#include "parser.h"

enum parser_status parser_pipeline(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    // Catch negation if there is
    struct ast *tmp_final = NULL;
    if (peek.type == TOKEN_NEG)
    {
        tmp_final = ast_new(AST_NEG);
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);
    }

    if (parser_command(lex, res) == PARSER_UNEXPECTED_TOKEN)
    {
        debug_printf(LOG_PARS,
                     "[PARSER] Failed parse first command - pipeline\n");
        ast_free(tmp_final);
        return PARSER_UNEXPECTED_TOKEN;
    }
    if (tmp_final != NULL)
        tmp_final->first_child = *res;
    else
        tmp_final = *res;

    // Parse optional { '|' {'\n'} command }
    peek = lexer_peek(lex);
    while (peek.type == TOKEN_PIPE)
    {
        struct ast *tmp_pipe = ast_new(AST_PIPE);
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);

        // Skip optional newline
        skip_newline(lex);

        peek = lexer_peek(lex);
        if (parser_command(lex, res) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_pipe);
            ast_free(tmp_final);
            return PARSER_UNEXPECTED_TOKEN;
        }

        // Create pipeline AST
        tmp_pipe->first_child = tmp_final;
        ast_add_brother(tmp_final, *res);
        tmp_final = tmp_pipe;

        peek = lexer_peek(lex);
    }

    debug_printf(LOG_PARS, "[PARSER] Quit parser pipeline\n");
    *res = tmp_final;
    return PARSER_OK;
}

enum parser_status parser_prefix(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    if (peek.family == TOKEN_FAM_ASSIGNMENT_W)
    {
        list_append(&((*res)->arg), peek.data);

        lexer_pop(lex);
        return PARSER_OK;
    }

    return parser_redirection(lex, res);
}

enum parser_status parser_redirection(struct lexer *lex, struct ast **res)
{
    struct token number = lexer_peek(lex);

    if (number.family == TOKEN_FAM_IO_NUMBER)
        lexer_pop(lex);

    // Check redirection token
    struct token redir = lexer_peek(lex);
    if (redir.family != TOKEN_FAM_REDIR)
        goto error;

    lexer_pop(lex);
    // Check word token
    struct token word = lexer_peek(lex);
    if (word.type != TOKEN_WORD)
    {
        token_free(redir);
        goto error;
    }
    lexer_pop(lex);

    *res = ast_new(AST_REDIR);
    (*res)->arg = list_create(redir.data);
    list_append(&((*res)->arg), word.data);

    if (number.family == TOKEN_FAM_IO_NUMBER)
        list_append(&((*res)->arg), number.data);
    return PARSER_OK;

error:
    // If there is IO_NUMBER, peek and number differ
    if (number.family == TOKEN_FAM_IO_NUMBER)
        token_free(number);
    token_free(lexer_pop(lex));
    return PARSER_UNEXPECTED_TOKEN;
}
