#include "parser.h"

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    // TOKEN_WORD -> first of simple_command
    if (peek.type == TOKEN_WORD)
        return parser_simple_command(lex, res);

    // Firsts of shell_command
    if (peek.type == TOKEN_IF || peek.type == TOKEN_WHILE || peek.type == TOKEN_UNTIL || peek.type == TOKEN_FOR)
    {
        debug_printf(LOG_PARS,"[PARSER] Shell command - command\n");
        return parser_shell_command(lex, res);
        //TODO There can is redirection
    }

    token_free(lexer_pop(lex));
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res)
{
    //TODO Add prefix rule
    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_WORD)
    {
        token_free(lexer_pop(lex));
        return PARSER_UNEXPECTED_TOKEN;
    }

    // Pop previous word
    peek = lexer_pop(lex);

    *res = ast_new(AST_COMMAND);
    (*res)->arg = list_create(peek.data);

    while (parser_element(lex, res) == PARSER_OK)
        continue;

    debug_printf(LOG_PARS, "[PARSER] Quit simple command\n");
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
    
    /*if (peek.type == TOKEN_FOR)
        return parser_rule_for(lex, res);*/
    
    token_free(lexer_pop(lex));
    return PARSER_UNEXPECTED_TOKEN;
}
