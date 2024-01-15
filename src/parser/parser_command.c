#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    // TOKEN_WORD -> first of simple_command
    if (peek.type == TOKEN_WORD)
    {
        debug_printf(LOG_PARS,"[PARSER] token WORD - command\n");
        return parser_simple_command(lex, res);
    }

    // TOKEN_IF -> first of shell_command
    if (peek.type == TOKEN_IF)
    {
        debug_printf(LOG_PARS,"[PARSER] token IF - command\n");
        return parser_shell_command(lex, res);
    }

    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_WORD)
        return PARSER_UNEXPECTED_TOKEN;

    peek = lexer_pop(lex);

    *res = ast_new(AST_COMMAND);
    (*res)->arg = list_create(peek.data);

    while (parser_element(lex, res) == PARSER_OK) { }

    return PARSER_OK;
}

enum parser_status parser_shell_command(struct lexer *lex, struct ast **res)
{
    return parser_rule_if(lex, res);
}
