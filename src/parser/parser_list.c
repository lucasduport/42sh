#include "parser.h"

enum parser_status parser_list(struct lexer *lex, struct ast **res)
{
    if (parser_and_or(lex, res) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;

    struct token peek = lexer_peek(lex);

    while (peek.type == TOKEN_SEMICOLONS)
    {
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);

        //';' is the last optional => EOF or '\n' -> follow of list
        if (peek.type == TOKEN_EOF || peek.type == TOKEN_NEWLINE)
        {
            struct ast *tmp = *res;
            *res = ast_new(AST_LIST);
            (*res)->first_child = tmp;

            return PARSER_OK;
        }

        struct ast *tmp_ast = NULL;
        // Else we need to parse 'and_or'
        if (parser_and_or(lex, &tmp_ast) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_ast);
            return PARSER_UNEXPECTED_TOKEN;
        }

        // If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);
    }
    struct ast *tmp = *res;
    *res = ast_new(AST_LIST);
    (*res)->first_child = tmp;

    debug_printf(LOG_PARS, "[PARSER] Quit list\n");
    return PARSER_OK;
}

enum parser_status parser_compound_list(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    while (peek.type == TOKEN_NEWLINE)
    {
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);
    }

    debug_printf(LOG_PARS, "[PARSER] Try parse first and_or - compund list\n");
    if (parser_and_or(lex, res) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;

    peek = lexer_peek(lex);

    while (peek.type == TOKEN_SEMICOLONS || peek.type == TOKEN_NEWLINE)
    {
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);

        // Skip '\n'
        while (peek.type == TOKEN_NEWLINE)
        {
            token_free(lexer_pop(lex));
            peek = lexer_peek(lex);
        }

        debug_printf(LOG_PARS, "[PARSER] peek = %s\n", peek.data);
        if (peek.type == TOKEN_ELSE || peek.type == TOKEN_ELIF
            || peek.type == TOKEN_THEN || peek.type == TOKEN_FI
            || peek.type == TOKEN_DONE || peek.type == TOKEN_DO)
        {
            struct ast *tmp = *res;
            *res = ast_new(AST_LIST);
            (*res)->first_child = tmp;
            return PARSER_OK;
        }

        struct ast *tmp_ast = NULL;
        // Else we need to parse 'and_or'
        if (parser_and_or(lex, &tmp_ast) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_ast);
            return PARSER_UNEXPECTED_TOKEN;
        }

        // If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);
    }

    struct ast *tmp = *res;
    *res = ast_new(AST_LIST);
    (*res)->first_child = tmp;

    return PARSER_OK;
}
