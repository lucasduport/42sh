#include "parser.h"

#include <stdio.h>

enum parser_status parser(int argc, char *argv[], struct ast **res)
{
    struct lexer *lexer = lexer_new(argc, argv);
    if (lexer == NULL)
        return PARSER_ARG_ERROR;

    return parser_input(lexer, res);
}

enum parser_status parser_input(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    *res = NULL;

    //first token = EOF or NEWLINE -> OK
    if (peek.type == TOKEN_EOF || peek.type == TOKEN_NEWLINE)
        return PARSER_OK;

    if (parser_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;

    // parse_list works => there must be an EOF or NEWLINE.
    peek = lexer_peek(lex);
    if (peek.type == TOKEN_EOF || peek.type == TOKEN_NEWLINE)
    {
        //Create ast RES with type AST_LIST
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
    return parser_pipeline(lex, res);
}

enum parser_status parser_pipeline(struct lexer *lex, struct ast **res)
{
    return parser_command(lex, res);
}

enum parser_status parser_element(struct lexer *lex)
{
    struct token peek = lexer_peek(lex);
    
    if (peek.type != TOKEN_SEMICOLONS && peek.type != TOKEN_NEWLINE && peek.type != TOKEN_EOF)
        return PARSER_OK;
    
    return PARSER_UNEXPECTED_TOKEN;
}
