#include "parser.h"

enum parser_status parser_rule_while(struct lexer *lex, struct ast **res)
{
    debug_printf(LOG_PARS, "[PARSER] In while rule\n");

    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_WHILE)
    {
        token_free(lexer_pop(lex));
        return PARSER_UNEXPECTED_TOKEN;
    }

    struct ast *tmp_final = ast_new(AST_WHILE);
    token_free(lexer_pop(lex));
    
    debug_printf(LOG_PARS, "[PARSER] Try to parse while condition\n");
    if (parser_compound_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;
    
    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DO)
    {
        debug_printf(LOG_PARS, "[PARSER] Fail parse TOKEN_DO\n");
        token_free(lexer_pop(lex));
        goto error;
    }
    token_free(lexer_pop(lex));
    
    tmp_final->first_child = *res;

    debug_printf(LOG_PARS, "[PARSER] Try to parse while execution\n");
    if (parser_compound_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;

    debug_printf(LOG_PARS, "[PARSER] Successfuly parse while execution\n");
    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DONE)
    {
        debug_printf(LOG_PARS, "[PARSER] Fail parse TOKEN_DONE\n");
        token_free(lexer_pop(lex));
        goto error;
    }
    token_free(lexer_pop(lex));

    ast_add_brother(tmp_final->first_child, *res);
    return PARSER_OK;

error:
    ast_free(tmp_final);
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_rule_until(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_UNTIL)
    {
        token_free(lexer_pop(lex));
        return PARSER_UNEXPECTED_TOKEN;
    }

    struct ast *tmp_final = ast_new(AST_UNTIL);
    token_free(lexer_pop(lex));
    
    if (parser_compound_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;
    
    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DO)
    {
        token_free(lexer_pop(lex));
        goto error;
    }
    
    tmp_final->first_child = *res;

    if (parser_compound_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;

    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DONE)
    {
        token_free(lexer_pop(lex));
        goto error;
    }
    
    ast_add_brother(tmp_final->first_child, *res);
    return PARSER_OK;

error:
    ast_free(tmp_final);
    return PARSER_UNEXPECTED_TOKEN;
}