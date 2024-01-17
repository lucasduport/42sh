#include "parser.h"

#include <stdio.h>

enum parser_status parser_input(struct lexer *lex, struct ast **res)
{
    enum token_type peek_type = (lexer_peek(lex)).type;
    *res = NULL;

    // first token = EOF or NEWLINE -> EOF or OK
    if (peek_type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
    {
        token_free(lexer_pop(lex));
        return (peek_type == TOKEN_EOF) ? PARSER_EOF : PARSER_OK;
    }

    if (parser_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;

    // parse_list works => there must be an EOF or NEWLINE.
    peek_type = (lexer_peek(lex)).type;
    if (peek_type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
    {
        token_free(lexer_pop(lex));
        return PARSER_OK;
    }

    // If not.
    goto error;

error:
    fprintf(stderr, "parser: parsing error\n");
    ast_free(*res);
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_and_or(struct lexer *lex, struct ast **res)
{
    if (parser_pipeline(lex, res) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;

    struct ast *tmp_final = *res;

    // Check optional { ('&&' || '||') {'\n'} pipeline }
    struct token peek = lexer_peek(lex);
    while (peek.type == TOKEN_AND_IF || peek.type == TOKEN_OR_IF)
    {
        debug_printf(LOG_PARS, "[PARSER] Enter in and/or loop\n");
        // Create 'and' or 'or' AST for after
        struct ast *tmp = ast_new((peek.type == TOKEN_AND_IF) ? AST_AND : AST_OR);
        token_free(lexer_pop(lex));

        peek = lexer_peek(lex);
        // Pop optional newline
        while (peek.type == TOKEN_NEWLINE)
        {
            token_free(lexer_pop(lex));
            peek = lexer_peek(lex);
        }

        if (parser_pipeline(lex, res) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp);
            ast_free(tmp_final);
            return PARSER_UNEXPECTED_TOKEN;
        }

        tmp->first_child = tmp_final;
        ast_add_brother(tmp_final, *res);
        tmp_final = tmp;

        peek = lexer_peek(lex);
        debug_printf(LOG_PARS, "[PARSER] End of and/or loop\n");
    }
    *res = tmp_final;
    return PARSER_OK;
}

enum parser_status parser_element(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.family != TOKEN_FAM_OPERATOR && peek.type != TOKEN_EOF)
    {
        peek = lexer_pop(lex);
        list_append((*res)->arg, peek.data);
        debug_printf(LOG_PARS, "[PARSER] Return element = %s\n", peek.data);
        return PARSER_OK;
    }

    return PARSER_UNEXPECTED_TOKEN;
}
