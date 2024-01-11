#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    //TOKEN_WORD -> first of simple_command
    if (peek.type == TOKEN_WORD)
        return parser_simple_command(lex, res);
    
    //TOKEN_IF -> first of shell_command
    if (peek.type == TOKEN_IF)
        return parser_shell_command(lex, res);
    
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
    while (peek.type != TOKEN_EOF && peek.type != TOKEN_NEWLINE && parser_element(lex) == PARSER_OK)
    {
        //pop because peek (and not pop) in element
        peek = lexer_pop(lex);
        list_append((*res)->arg, peek.data);
    }

    //If while boucle exit properly (with EOF or '\n')
    if (peek.type != TOKEN_EOF && peek.type != TOKEN_NEWLINE)
        return PARSER_OK;
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_shell_command(struct lexer *lex, struct ast **res)
{
    return parser_rule_if(lex, res);
}
