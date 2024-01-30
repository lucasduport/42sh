#include "parser.h"

static int is_first_shell(struct token peek)
{
    return peek.type == TOKEN_IF || peek.type == TOKEN_WHILE
             || peek.type == TOKEN_UNTIL || peek.type == TOKEN_FOR
             || peek.type == TOKEN_LEFT_BRACE || peek.type == TOKEN_LEFT_PAR
             || peek.type == TOKEN_CASE;
}

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    if (peek.type == TOKEN_ERROR)
        return token_free(lexer_pop(lex)), PARSER_ERROR;

    // Firsts of shell_command
    else if (is_first_shell(peek))
    {
        if (parser_shell_command(lex, res) == PARSER_ERROR)
            return PARSER_ERROR;

        struct ast *tmp_command = *res;
        struct ast *tmp_redir = NULL;

        peek = lexer_peek(lex);
        while (peek.family != TOKEN_FAM_OPERATOR)
        {
            if (parser_redirection(lex, res) == PARSER_ERROR)
            {
                ast_free(tmp_command);
                ast_free(tmp_redir);
                return PARSER_ERROR;
            }

            ast_add_child_to_child(&tmp_redir, *res);
            peek = lexer_peek(lex);
        }
        if (tmp_redir != NULL)
        {
            ast_add_child_to_child(&tmp_redir, *res);
            *res = tmp_redir;
        }
        return PARSER_OK;
    }
    else
    {
        if (peek.type == TOKEN_WORD && peek.family != TOKEN_FAM_ASSW)
        {
            struct token word = peek;
            lexer_pop(lex);
            peek = lexer_peek(lex);
            if (peek.type == TOKEN_LEFT_PAR)
                return parser_fundec(lex, res, &word);
            return parser_simple_command(lex, res, &word);
        }
        return parser_simple_command(lex, res, NULL);
    }
}

/**
 * @brief Catch error in simple_command
 *
 * If lexer != NULL => free lexer_pop
 */
static enum parser_status error_sc(struct lexer *lex, struct ast *redir,
                                   struct ast *assignment, struct ast *command)
{
    if (lex != NULL)
        token_free(lexer_pop(lex));
    ast_free(redir);
    ast_free(assignment);
    ast_free(command);
    return PARSER_ERROR;
}

/**
 * @brief Parse prefix loop in simple_command
 */
static enum parser_status sub_prefix_loop(struct lexer *lex, struct ast **res,
                                          struct ast **assignment,
                                          struct ast **redir)
{
    struct token peek = lexer_peek(lex);
    while (peek.family == TOKEN_FAM_IO_NUMBER || peek.family == TOKEN_FAM_REDIR
           || peek.family == TOKEN_FAM_ASSW)
    {
        if (parser_prefix(lex,
                          (peek.family == TOKEN_FAM_ASSW) ? assignment : res)
            == PARSER_ERROR)
            return PARSER_ERROR;

        if (peek.family != TOKEN_FAM_ASSW)
            ast_add_child_to_child(redir, *res);
        peek = lexer_peek(lex);
    }
    if (peek.type == TOKEN_ERROR)
        return token_free(lexer_pop(lex)), PARSER_ERROR;
    return PARSER_OK;
}

/**
 * @brief Link correctly all trees if no command was parsing in simple command
 */
static enum parser_status sub_no_command(struct lexer *lex, struct ast **res,
                                         struct ast **assignment,
                                         struct ast **redir)
{
    *res = *redir;
    // Minimum one prefix
    if (*redir == NULL && (*assignment)->arg == NULL)
        return token_free(lexer_pop(lex)), PARSER_ERROR;

    if ((*assignment)->arg == NULL)
        ast_free(*assignment);
    else
        ast_add_child_to_child(redir, *assignment);
    *res = *redir;
    return PARSER_OK;
}

/**
 * @brief Link all sub-trees at the end of simple_command
 */
static void link_redir_command(struct ast **res, struct ast **assignment,
                               struct ast **redir, struct ast **command)
{
    // No assignment => redirection + command
    if ((*assignment)->arg == NULL)
    {
        ast_free(*assignment);
        ast_add_child_to_child(redir, *command);
        *res = *redir;
    }
    else
    {
        // No redirection => assignment + command
        if (*redir == NULL)
            ast_add_child_to_child(assignment, *command);
        // Redirection => assignment + redirection + command
        else
        {
            ast_add_child_to_child(assignment, *redir);
            (*redir)->first_child = *command;
        }
        *res = *assignment;
    }
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res,
                                         struct token *w)
{
    struct token peek = lexer_peek(lex);
    if (peek.type == TOKEN_ERROR)
    {
        if (w != NULL)
            token_free(*w);
        return token_free(lexer_pop(lex)), PARSER_ERROR;
    }

    struct ast *redir = NULL;
    struct ast *assignment = ast_new(AST_ASSIGNMENT);
    struct ast *command = ast_new(AST_COMMAND);

    // Word or prefix loop
    if (w != NULL)
        command->arg = list_create(w->data);
    else if (sub_prefix_loop(lex, res, &assignment, &redir) == PARSER_ERROR)
        return error_sc(NULL, redir, assignment, command);

    peek = lexer_peek(lex);
    if (peek.type == TOKEN_ERROR)
        return error_sc(lex, redir, assignment, command);

    // No command
    if (w == NULL && peek.type != TOKEN_WORD)
    {
        if (sub_no_command(lex, res, &assignment, &redir))
            return error_sc(NULL, redir, assignment, command);

        ast_free(command);
        return PARSER_OK;
    }

    // Pop previous word
    if (w == NULL)
    {
        peek = lexer_pop(lex);
        command->arg = list_create(peek.data);
    }

    peek = lexer_peek(lex);
    while (peek.family != TOKEN_FAM_OPERATOR && peek.type != TOKEN_RIGHT_PAR
           && peek.type != TOKEN_ERROR)
    {
        if (peek.family == TOKEN_FAM_IO_NUMBER
            || peek.family == TOKEN_FAM_REDIR)
        {
            if (parser_element(lex, res) == PARSER_ERROR)
                return error_sc(NULL, redir, assignment, command);

            ast_add_child_to_child(&redir, *res);
        }
        else
            parser_element(lex, &command);
        peek = lexer_peek(lex);
    }

    if (peek.type == TOKEN_ERROR)
        return error_sc(lex, redir, assignment, command);

    // Link redir and command
    link_redir_command(res, &assignment, &redir, &command);

    return PARSER_OK;
}

static enum parser_status sub_parse_par(struct lexer *lex, struct ast **res)
{
    token_free(lexer_pop(lex));
    if (parser_compound_list(lex, res) == PARSER_ERROR)
        return PARSER_ERROR;
    if (lexer_peek(lex).type != TOKEN_RIGHT_PAR)
    {
        token_free(lexer_pop(lex));
        return PARSER_ERROR;
    }
    token_free(lexer_pop(lex));
    struct ast *tmp_ast = ast_new(AST_SUBSHELL);
    tmp_ast->first_child = *res;
    *res = tmp_ast;
    return PARSER_OK;
}

static enum parser_status sub_parse_brace(struct lexer *lex, struct ast **res)
{
    token_free(lexer_pop(lex));
    if (parser_compound_list(lex, res) == PARSER_ERROR)
        return PARSER_ERROR;
    if (lexer_peek(lex).type != TOKEN_RIGHT_BRACE)
    {
        token_free(lexer_pop(lex));
        return PARSER_ERROR;
    }
    token_free(lexer_pop(lex));
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

    if (peek.type == TOKEN_FOR)
        return parser_rule_for(lex, res);

    if (peek.type == TOKEN_LEFT_PAR)
        return sub_parse_par(lex, res);

    if (peek.type == TOKEN_LEFT_BRACE)
        return sub_parse_brace(lex, res);
    
    if (peek.type == TOKEN_CASE)
        return parser_rule_case(lex, res);

    token_free(lexer_pop(lex));
    return PARSER_ERROR;
}

enum parser_status parser_fundec(struct lexer *lex, struct ast **res,
                                 struct token *w)
{
    struct ast *tmp_res = ast_new(AST_FUNC);
    tmp_res->arg = list_create(w->data);

    struct token peek = lexer_peek(lex);
    token_free(lexer_pop(lex));
    if (peek.type != TOKEN_LEFT_PAR)
        goto error;

    peek = lexer_peek(lex);
    token_free(lexer_pop(lex));
    if (peek.type != TOKEN_RIGHT_PAR)
        goto error;

    skip_newline(lex);
    if (parser_shell_command(lex, res) == PARSER_ERROR)
        goto error;
    tmp_res->first_child = *res;
    *res = tmp_res;
    return PARSER_OK;

error:
    ast_free(tmp_res);
    return PARSER_ERROR;
}
