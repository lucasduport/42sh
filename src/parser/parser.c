#include "parser.h"

#include <stdio.h>

enum parser_status parser_input(struct lexer *lex, struct ast **res)
{
    enum token_type peek_type = (lexer_peek(lex)).type;
    *res = NULL;

    // first token = EOF or NEWLINE -> OK
    if (peek_type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
    {
        token_free(lexer_pop(lex));
        return (peek_type == TOKEN_EOF) ? PARSER_EOF_VALID : PARSER_OK;
    }

    if (parser_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;

    debug_printf(LOG_PARS, "[PARSER] Finish parser input\n");
    // parse_list works => there must be an EOF or NEWLINE.
    peek_type = (lexer_peek(lex)).type;
    if (peek_type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
    {
        token_free(lexer_pop(lex));
        return (peek_type == TOKEN_EOF) ? PARSER_EOF_VALID : PARSER_OK;
    }

    // If not.
    goto error;

error:
    peek_type = (lexer_peek(lex)).type;
    if (peek_type == TOKEN_EOF)
        token_free(lexer_pop(lex));
    fprintf(stderr, "parser: parsing error\n");
    ast_free(*res);
    return (peek_type == TOKEN_EOF) ? PARSER_EOF_ERROR : PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_and_or(struct lexer *lex, struct ast **res)
{
    return parser_pipeline(lex, res);
}

enum parser_status parser_pipeline(struct lexer *lex, struct ast **res)
{
    return parser_command(lex, res);
}

enum parser_status parser_element(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_SEMICOLONS && peek.type != TOKEN_NEWLINE 
        && peek.type != TOKEN_EOF)
    {
        peek = lexer_pop(lex);
        list_append((*res)->arg, peek.data);
        debug_printf(LOG_PARS, "[PARSER] Return element = %s\n", peek.data);
        return PARSER_OK;
    }

    return PARSER_UNEXPECTED_TOKEN;
}
