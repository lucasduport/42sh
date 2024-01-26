#include "parser.h"

enum parser_status parser_rule_while(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_WHILE)
    {
        token_free(lexer_pop(lex));
        return PARSER_ERROR;
    }

    struct ast *tmp_final = ast_new(AST_WHILE);
    token_free(lexer_pop(lex));

    if (parser_compound_list(lex, res) == PARSER_ERROR)
        goto error;

    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DO)
    {
        token_free(lexer_pop(lex));
        goto error;
    }
    token_free(lexer_pop(lex));

    tmp_final->first_child = *res;

    if (parser_compound_list(lex, res) == PARSER_ERROR)
        goto error;

    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DONE)
    {
        token_free(lexer_pop(lex));
        goto error;
    }
    token_free(lexer_pop(lex));

    ast_add_brother(tmp_final->first_child, *res);
    *res = tmp_final;
    return PARSER_OK;

error:
    ast_free(tmp_final);
    return PARSER_ERROR;
}

enum parser_status parser_rule_until(struct lexer *lex, struct ast **res)
{
    debug_printf(LOG_PARS, "[PARSER] In until rule\n");

    struct token peek = lexer_peek(lex);

    if (peek.type != TOKEN_UNTIL)
    {
        token_free(lexer_pop(lex));
        return PARSER_ERROR;
    }

    struct ast *tmp_final = ast_new(AST_UNTIL);
    token_free(lexer_pop(lex));

    if (parser_compound_list(lex, res) == PARSER_ERROR)
        goto error;

    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DO)
    {
        token_free(lexer_pop(lex));
        goto error;
    }
    token_free(lexer_pop(lex));

    tmp_final->first_child = *res;

    if (parser_compound_list(lex, res) == PARSER_ERROR)
        goto error;

    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DONE)
    {
        token_free(lexer_pop(lex));
        goto error;
    }
    token_free(lexer_pop(lex));

    ast_add_brother(tmp_final->first_child, *res);
    *res = tmp_final;
    return PARSER_OK;

error:
    ast_free(tmp_final);
    return PARSER_ERROR;
}

/**
 * @brief Check if there is semi-colons
 *
 * @return Return 1 if there is, 0 otherwise
 */
static int check_semicolons(struct lexer *lex)
{
    struct token peek = lexer_peek(lex);
    if (peek.type == TOKEN_SEMICOLONS)
    {
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);
        return 1;
    }
    return 0;
}

/**
 * @brief Help to parse 'for' loop if ther is not semicolons after condition
 *
 * @param lex Lexer currently used
 * @param tmp_final Final ast to which we add the arguments
 */
static enum parser_status sub_parse_for_nosemi(struct lexer *lex,
                                               struct ast *tmp_final)
{
    struct token peek = lexer_peek(lex);
    if (peek.type == TOKEN_ERROR)
        goto error;
    
    if (peek.type == TOKEN_IN)
    {
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);

        while (peek.type == TOKEN_WORD)
        {
            list_append(&tmp_final->arg, peek.data);
            lexer_pop(lex);
            peek = lexer_peek(lex);
        }

        if (peek.type != TOKEN_NEWLINE && peek.type != TOKEN_SEMICOLONS)
        {
            ast_free(tmp_final);
            goto error;
        }
        token_free(lexer_pop(lex));
    }
    return PARSER_OK;
    
error:
    token_free(lexer_pop(lex));
    return PARSER_ERROR;
}

enum parser_status parser_rule_for(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    struct ast *tmp_final = NULL;

    // Check for
    if (peek.type != TOKEN_FOR)
        goto error;

    token_free(lexer_pop(lex));

    // Check word
    peek = lexer_peek(lex);
    if (peek.family != TOKEN_FAM_WORD && peek.family != TOKEN_FAM_RESERVED)
        goto error;
    lexer_pop(lex);

    // Construct AST
    tmp_final = ast_new(AST_FOR);
    tmp_final->arg = list_create(peek.data);

    // Check semi-colons
    int there_is_semicol = check_semicolons(lex);

    skip_newline(lex);

    // Case 'in' or 'do'
    if (!there_is_semicol
        && sub_parse_for_nosemi(lex, tmp_final) == PARSER_ERROR)
        return PARSER_ERROR;

    skip_newline(lex);

    // Parse do
    peek = lexer_peek(lex);
    if (peek.type != TOKEN_DO)
        goto error;

    token_free(lexer_pop(lex));

    if (parser_compound_list(lex, res) == PARSER_ERROR)
    {
        ast_free(tmp_final);
        return PARSER_ERROR;
    }

    peek = lexer_peek(lex);

    // Parse done
    if (peek.type != TOKEN_DONE)
        goto error;

    token_free(lexer_pop(lex));
    tmp_final->first_child = *res;
    *res = tmp_final;
    return PARSER_OK;

error:
    ast_free(tmp_final);
    token_free(lexer_pop(lex));
    return PARSER_ERROR;
}
