#include "parser.h"

#include <stdio.h>

enum parser_status parser_input(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    enum token_type peek_type = peek.type;
    *res = NULL;

    // first token = EOF or NEWLINE -> OK
    if (peek_type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
    {
        token_free(peek);
        token_free(lexer_pop(lex));
        return (peek_type == TOKEN_EOF) ? PARSER_EOF_VALID : PARSER_OK;
    }

    token_free(peek);
    if (parser_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;

    // parse_list works => there must be an EOF or NEWLINE.
    peek = lexer_peek(lex);
    peek_type = peek.type;
    if (peek_type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
    {
        token_free(peek);
        token_free(lexer_pop(lex));
        return (peek_type == TOKEN_EOF) ? PARSER_EOF_VALID : PARSER_OK;
    }

    // If not.
    token_free(peek);
    goto error;

error:
    peek = lexer_peek(lex);
    peek_type = peek.type;
    fprintf(stderr, "parser: parsing error\n");
    token_free(peek);
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
        token_free(lexer_pop(lex));
        list_append((*res)->arg, peek.data);
        return PARSER_OK;
    }

    token_free(peek);
    debug_printf(LOG_PARS,"[PARSER] It's not an element\n");
    return PARSER_UNEXPECTED_TOKEN;
}
