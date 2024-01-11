#include "parser.h"

enum parser_status parser_list(struct lexer *lex, struct ast **res)
{
    if (parser_and_or(lex, res) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    
    struct token peek = lexer_peek(lex);

    while (peek.type == TOKEN_SEMICOLONS)
    {
        lexer_pop(lex);
        peek = lexer_peek(lex);

        //';' is the last optional => EOF or '\n' -> follow of list
        if (peek.type == TOKEN_EOF || peek.type == TOKEN_NEWLINE)
            return PARSER_OK;
        
        struct ast *tmp_ast = NULL;
        //Else we need to parse 'and_or'
        if (parser_and_or(lex, &tmp_ast) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_ast);
            return PARSER_UNEXPECTED_TOKEN;
        }
    
        //If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);

    }
    return PARSER_OK;
}

enum parser_status parser_compound_list(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    while (peek.type == TOKEN_NEWLINE)
    {
        lexer_pop(lex);
        peek = lexer_peek(lex);
    }

    if (parser_and_or(lex, res) == PARSER_UNEXPECTED_TOKEN)
    {
        debug_printf("[PARSER] Failed parse first and_or - compound_list");
        return PARSER_UNEXPECTED_TOKEN;
    }
    
    peek = lexer_peek(lex);

    while (peek.type == TOKEN_SEMICOLONS || peek.type == TOKEN_NEWLINE)
    {
        lexer_pop(lex);
        peek = lexer_peek(lex);

        //Skip '\n'
        while(peek.type == TOKEN_NEWLINE)
        {
            lexer_pop(lex);
            peek = lexer_peek(lex);
        }

        if(peek.type == TOKEN_ELSE || peek.type == TOKEN_ELIF || peek.type == TOKEN_THEN || peek.type == TOKEN_FI)
            return PARSER_OK;
        
        struct ast *tmp_ast = NULL;
        //Else we need to parse 'and_or'
        if (parser_and_or(lex, &tmp_ast) == PARSER_UNEXPECTED_TOKEN)
        {
            debug_printf("[PARSER] Failed parse optional and_or - compound_list");
            ast_free(tmp_ast);
            return PARSER_UNEXPECTED_TOKEN;
        }
    
        //If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);
    }
    return PARSER_OK;
}
