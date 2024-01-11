#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    //TOKEN_WORD -> first of simple_command
    if (peek.type == TOKEN_WORD)
    {
        debug_printf("[PARSER] token WORD - command");
        return parser_simple_command(lex, res);
    }
    
    //TOKEN_IF -> first of shell_command
    if (peek.type == TOKEN_IF)
    {
        debug_printf("[PARSER] token IF - command");
        return parser_shell_command(lex, res);
    }
    
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_WORD)
        return PARSER_UNEXPECTED_TOKEN;
    lexer_pop(lex);

    *res = ast_new(AST_COMMAND);
    (*res)->arg = list_create(peek.data);

    peek = lexer_peek(lex);
    //EOF and '\n' -> follow of parser_simple_command
    while (parser_element(lex) == PARSER_OK)
    {
        //pop because peek (and not pop) in element
        peek = lexer_pop(lex);
        list_append((*res)->arg, peek.data);
    }

    return PARSER_OK;
}

enum parser_status parser_shell_command(struct lexer *lex, struct ast **res)
{
    return parser_rule_if(lex, res);
}
