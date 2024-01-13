#include "parser.h"

/**
 * @brief Sub function to parse then_clause
 *
 * @param lex Lexer used in current execution
 * @param tmp_condition Ast that contains condition of the if concern by
 * else_parsing
 * @return parser_status
 */
static enum parser_status sub_parse_then(struct lexer *lex,
                                         struct ast *tmp_condition)
{
    struct token peek = lexer_peek(lex);
    if (peek.type != TOKEN_THEN)
    {
        debug_printf("[PARSER] Failed parse 'then' token\n");
        token_free(peek);
        return PARSER_UNEXPECTED_TOKEN;
    }

    token_free(peek);
    token_free(lexer_pop(lex));

    struct ast *tmp_then = NULL;

    if (parser_compound_list(lex, &tmp_then) == PARSER_UNEXPECTED_TOKEN)
    {
        debug_printf("[PARSER] Failed parse 'then' list\n");
        ast_free(tmp_then);
        return PARSER_UNEXPECTED_TOKEN;
    }

    // add brother at tmp_condition (and not res) because that is IF node that
    // have different child
    ast_add_brother(tmp_condition, tmp_then);

    return PARSER_OK;
}

/**
 * @brief Sub function to parse else_clause
 *
 * @param lex Lexer used in current execution
 * @param tmp_condition Ast that contains condition of the if concern by
 * else_parsing
 * @return parser_status
 */
static enum parser_status sub_parse_else(struct lexer *lex,
                                         struct ast *tmp_condition)
{
    struct token peek = lexer_peek(lex);
    if (peek.type == TOKEN_ELSE || peek.type == TOKEN_ELIF)
    {
        struct ast *tmp_else = NULL;
        if (parser_else_clause(lex, &tmp_else) == PARSER_UNEXPECTED_TOKEN)
        {
            token_free(peek);
            debug_printf("[PARSER] Failed parse else_clause\n");
            ast_free(tmp_else);
            return PARSER_UNEXPECTED_TOKEN;
        }

        ast_add_brother(tmp_condition, tmp_else);
    }
    token_free(peek);
    return PARSER_OK;
}

enum parser_status parser_rule_if(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type == TOKEN_IF)
    {
        token_free(peek);
        token_free(lexer_pop(lex));
        *res = ast_new(AST_IF);

        // PARSE CONDITION
        struct ast *tmp_condition = NULL;

        if (parser_compound_list(lex, &tmp_condition)
            == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_condition);
            return PARSER_UNEXPECTED_TOKEN;
        }

        (*res)->first_child = tmp_condition;

        // PARSE THEN
        if (sub_parse_then(lex, tmp_condition) == PARSER_UNEXPECTED_TOKEN)
            return PARSER_UNEXPECTED_TOKEN;

        // PARSE ELSE (IF THERE IS)
        if (sub_parse_else(lex, tmp_condition) == PARSER_UNEXPECTED_TOKEN)
            return PARSER_UNEXPECTED_TOKEN;

        // PARSE FI
        peek = lexer_peek(lex);
        if (peek.type == TOKEN_FI)
        {
            token_free(peek);
            token_free(lexer_pop(lex));
            return PARSER_OK;
        }
    }
    token_free(peek);
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_else_clause(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    if (peek.type == TOKEN_ELSE)
    {
        token_free(peek);
        token_free(lexer_pop(lex));
        return parser_compound_list(lex, res);
    }

    else if (peek.type == TOKEN_ELIF)
    {
        token_free(peek);
        token_free(lexer_pop(lex));
        *res = ast_new(AST_IF);

        // PARSE CONDITION
        struct ast *tmp_condition = NULL;

        if (parser_compound_list(lex, &tmp_condition)
            == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_condition);
            return PARSER_UNEXPECTED_TOKEN;
        }

        (*res)->first_child = tmp_condition;

        // PARSE THEN
        if (sub_parse_then(lex, tmp_condition) == PARSER_UNEXPECTED_TOKEN)
            return PARSER_UNEXPECTED_TOKEN;

        // PARSE ELSE (IF THERE IS)
        if (sub_parse_else(lex, tmp_condition) == PARSER_UNEXPECTED_TOKEN)
            return PARSER_UNEXPECTED_TOKEN;

        return PARSER_OK;
    }

    token_free(peek);
    return PARSER_UNEXPECTED_TOKEN;
}
