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

    // It doesn't matter if it fails or not, we need to pop actual token
    token_free(lexer_pop(lex));

    if (peek.type != TOKEN_THEN)
    {
        debug_printf(LOG_PARS, "[PARSER] Failed parse 'then' token\n");
        return PARSER_ERROR;
    }

    struct ast *tmp_then = NULL;

    if (parser_compound_list(lex, &tmp_then) == PARSER_ERROR)
    {
        debug_printf(LOG_PARS, "[PARSER] Failed parse 'then' list\n");
        ast_free(tmp_then);
        return PARSER_ERROR;
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
    if (peek.type == TOKEN_ERROR)
        return token_free(lexer_pop(lex)), PARSER_ERROR;

    else if (peek.type == TOKEN_ELSE || peek.type == TOKEN_ELIF)
    {
        struct ast *tmp_else = NULL;
        if (parser_else_clause(lex, &tmp_else) == PARSER_ERROR)
        {
            debug_printf(LOG_PARS, "[PARSER] Failed parse else_clause\n");
            ast_free(tmp_else);
            return PARSER_ERROR;
        }

        ast_add_brother(tmp_condition, tmp_else);
    }
    return PARSER_OK;
}

enum parser_status parser_rule_if(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type == TOKEN_IF)
    {
        token_free(lexer_pop(lex));
        *res = ast_new(AST_IF);

        // PARSE CONDITION
        struct ast *tmp_condition = NULL;

        if (parser_compound_list(lex, &tmp_condition) == PARSER_ERROR)
        {
            ast_free(tmp_condition);
            return PARSER_ERROR;
        }

        (*res)->first_child = tmp_condition;

        // PARSE THEN
        if (sub_parse_then(lex, tmp_condition) == PARSER_ERROR)
            return PARSER_ERROR;

        // PARSE ELSE (IF THERE IS)
        if (sub_parse_else(lex, tmp_condition) == PARSER_ERROR)
            return PARSER_ERROR;

        // PARSE FI
        peek = lexer_peek(lex);
        if (peek.type == TOKEN_FI)
        {
            token_free(lexer_pop(lex));
            return PARSER_OK;
        }
    }
    token_free(lexer_pop(lex));
    return PARSER_ERROR;
}

enum parser_status parser_else_clause(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    if (peek.type == TOKEN_ELSE)
    {
        token_free(lexer_pop(lex));
        return parser_compound_list(lex, res);
    }

    else if (peek.type == TOKEN_ELIF)
    {
        token_free(lexer_pop(lex));
        *res = ast_new(AST_IF);

        // PARSE CONDITION
        struct ast *tmp_condition = NULL;

        if (parser_compound_list(lex, &tmp_condition) == PARSER_ERROR)
        {
            ast_free(tmp_condition);
            return PARSER_ERROR;
        }

        (*res)->first_child = tmp_condition;

        // PARSE THEN
        if (sub_parse_then(lex, tmp_condition) == PARSER_ERROR)
            return PARSER_ERROR;

        // PARSE ELSE (IF THERE IS)
        if (sub_parse_else(lex, tmp_condition) == PARSER_ERROR)
            return PARSER_ERROR;

        return PARSER_OK;
    }

    token_free(lexer_pop(lex));
    return PARSER_ERROR;
}

enum parser_status parser_rule_case(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    token_free(lexer_pop(lex));
    if (peek.type != TOKEN_CASE)
        return PARSER_ERROR;

    peek = lexer_peek(lex);
    if (peek.type != TOKEN_WORD && peek.family != TOKEN_FAM_RESERVED)
    {
        token_free(lexer_pop(lex));
        return PARSER_ERROR;
    }

    struct ast *tmp_case = ast_new(AST_CASE);
    tmp_case->arg = list_create(peek.data);
    lexer_pop(lex);

    skip_newline(lex);

    peek = lexer_peek(lex);
    token_free(lexer_pop(lex));
    if (peek.type != TOKEN_IN)
        goto error;

    skip_newline(lex);

    peek = lexer_peek(lex);
    if (peek.type != TOKEN_ESAC
        && parser_case_clause(lex, &tmp_case) == PARSER_ERROR)
        goto error;

    peek = lexer_peek(lex);
    token_free(lexer_pop(lex));

    if (peek.type != TOKEN_ESAC)
        goto error;

    *res = tmp_case;
    return PARSER_OK;

error:
    ast_free(tmp_case);
    return PARSER_ERROR;
}

enum parser_status parser_case_clause(struct lexer *lex, struct ast **res)
{
    if (parser_case_item(lex, res) == PARSER_ERROR)
        return PARSER_ERROR;

    struct token peek = lexer_peek(lex);
    while (peek.type == TOKEN_DSEMI)
    {
        token_free(lexer_pop(lex));
        skip_newline(lex);

        peek = lexer_peek(lex);
        if (peek.type == TOKEN_ESAC)
            return PARSER_OK;

        if (parser_case_item(lex, res) == PARSER_ERROR)
            return PARSER_ERROR;

        peek = lexer_peek(lex);
    }

    skip_newline(lex);

    if (peek.type == TOKEN_ERROR)
        return PARSER_ERROR;

    return PARSER_OK;
}

static enum parser_status construct_case(struct ast **res, struct ast *clause)
{
    if ((*res)->first_child == NULL)
        (*res)->first_child = clause;
    else
        ast_add_brother((*res)->first_child, clause);
    return PARSER_OK;
}

enum parser_status parser_case_item(struct lexer *lex, struct ast **res)
{
    struct ast *child = ast_new(AST_ITEM);
    struct token peek = lexer_peek(lex);

    if (peek.type == TOKEN_LEFT_PAR)
    {
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);
    }

    if (peek.type != TOKEN_WORD && peek.family != TOKEN_FAM_RESERVED)
        goto error;

    // Create the ITEM with its first pattern
    child->arg = list_create(peek.data);
    lexer_pop(lex);

    // While there is a pattern
    peek = lexer_peek(lex);
    while (peek.type == TOKEN_PIPE)
    {
        token_free(lexer_pop(lex));

        // Get the pattern
        peek = lexer_peek(lex);
        if (peek.type != TOKEN_WORD && peek.family != TOKEN_FAM_RESERVED)
            goto error;
        list_append(&child->arg, peek.data);
        lexer_pop(lex);

        peek = lexer_peek(lex);
    }

    if (peek.type != TOKEN_RIGHT_PAR)
        goto error;

    token_free(lexer_pop(lex));
    skip_newline(lex);

    peek = lexer_peek(lex);
    if (peek.type == TOKEN_DSEMI || peek.type == TOKEN_ESAC)
        return construct_case(res, child);

    struct ast *command = NULL;
    if (parser_compound_list(lex, &command) == PARSER_ERROR)
    {
        ast_free(command);
        goto error;
    }

    child->first_child = command;
    return construct_case(res, child);

error:
    token_free(lexer_pop(lex));
    ast_free(child);
    return PARSER_ERROR;
}
