#include "parser.h"

enum parser_status parser_list(struct lexer *lex, struct ast **res)
{
    if (parser_and_or(lex, res) == PARSER_ERROR)
        return PARSER_ERROR;

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
        if (parser_and_or(lex, &tmp_ast) == PARSER_ERROR)
        {
            ast_free(tmp_ast);
            return PARSER_ERROR;
        }

        // If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);
    }

    if (peek.type == TOKEN_ERROR)
        return token_free(lexer_pop(lex)), PARSER_ERROR;

    struct ast *tmp = *res;
    *res = ast_new(AST_LIST);
    (*res)->first_child = tmp;

    return PARSER_OK;
}

static int is_follow_complist(struct token tok)
{
    enum token_type follows[] = { TOKEN_ELIF, TOKEN_ELSE,
        TOKEN_THEN, TOKEN_FI, TOKEN_DONE, TOKEN_DO, TOKEN_RIGHT_BRACE,
        TOKEN_RIGHT_PAR};
    
    for (size_t i = 0; i < 8; i++)
    {
        if (tok.type == follows[i])
            return 1;
    }
    return 0;
}

enum parser_status parser_compound_list(struct lexer *lex, struct ast **res)
{
    skip_newline(lex);

    if (parser_and_or(lex, res) == PARSER_ERROR)
        return PARSER_ERROR;

    struct token peek = lexer_peek(lex);

    while (peek.type == TOKEN_SEMICOLONS || peek.type == TOKEN_NEWLINE)
    {
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);

        // Skip optional newline
        skip_newline(lex);

        peek = lexer_peek(lex);
        debug_printf(LOG_PARS, "[PARSER] peek = %s\n", peek.data);
        if (is_follow_complist(peek))
        {
            struct ast *tmp = *res;
            *res = ast_new(AST_LIST);
            (*res)->first_child = tmp;
            return PARSER_OK;
        }

        struct ast *tmp_ast = NULL;
        // Else we need to parse 'and_or'
        if (parser_and_or(lex, &tmp_ast) == PARSER_ERROR)
        {
            ast_free(tmp_ast);
            return PARSER_ERROR;
        }

        // If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);
    }

    if (peek.type == TOKEN_ERROR)
        return token_free(lexer_pop(lex)), PARSER_ERROR;

    struct ast *tmp = *res;
    *res = ast_new(AST_LIST);
    (*res)->first_child = tmp;

    return PARSER_OK;
}
