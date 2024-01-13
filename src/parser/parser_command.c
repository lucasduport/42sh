#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    //TOKEN_WORD -> first of simple_command
    if (peek.type == TOKEN_WORD)
    {
        token_free(peek);
        debug_printf("[PARSER] token WORD - command\n");
        return parser_simple_command(lex, res);
    }
    
    //TOKEN_IF -> first of shell_command
    if (peek.type == TOKEN_IF)
    {
        token_free(peek);
        debug_printf("[PARSER] token IF - command\n");
        return parser_shell_command(lex, res);
    }
    
    token_free(peek);
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_WORD)
    {
        token_free(peek);
        return PARSER_UNEXPECTED_TOKEN;
    }

    token_free(lexer_pop(lex));

    *res = ast_new(AST_COMMAND);
    (*res)->arg = list_create(peek.data);

    while (parser_element(lex, res) == PARSER_OK)
        continue;

    return PARSER_OK;
}

enum parser_status parser_shell_command(struct lexer *lex, struct ast **res)
{
    return parser_rule_if(lex, res);
}
