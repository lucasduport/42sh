#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "lexer.h"

/**
 * @brief Creates a new token
 *
 * @param type The type of the token
 * @param lexer
 * @return The new token
 */
static struct token token_alloc(enum token_type type, enum token_family family,
                                struct lexer *lexer)
{
    struct token token;
    token.data = calloc(lexer->current_word->len, sizeof(char));
    token.data =
        memcpy(token.data, lexer->current_word->data, lexer->current_word->len);
    token.type = type;
    token.family = family;

    string_reset(lexer->current_word);

    print_token(token);
    return token;
}

/**
 * @brief Check if the current word could be an IO number
 *
 * @param lexer The lexer
 */
static int check_io_number(struct lexer *lexer)
{
    for (size_t i = 0; lexer->current_word->data[i] != '\0'; i++)
    {
        if (!isdigit(lexer->current_word->data[i]))
            return 0;
    }

    return lexer->current_char == '<' || lexer->current_char == '>';
}

static int check_special_variable(const char *name)
{
    char special_char[] = { '$', '?', '@', '*', '#', '!', '.', '-', '+' };

    for (size_t i = 0; name[i] != '\0'; i++)
    {
        for (size_t j = 0; j < sizeof(special_char) / sizeof(char); j++)
        {
            if (name[i] == special_char[j])
                return 1;
        }
    }
    return 0;
}

/**
 * @brief Check if the current word could be an assignment word
 *
 * a=2 ?
 *
 * @param lexer The lexer
 */
static int check_assignment(struct lexer *lexer)
{
    int contains_equal = 0;
    for (size_t i = 0; i < lexer->current_word->len; i++)
    {
        if (lexer->current_word->data[i] == '=')
            contains_equal++;
    }

    if (contains_equal >= 1)
    {
        if (lexer->current_word->data[0] == '=')
            return 0;

        char *word_cpy = strdup(lexer->current_word->data);
        char *variable_name = strtok(word_cpy, "=");

        int code = 0;

        if (variable_name == NULL)
            code = 0;

        else if (check_special_variable(variable_name))
            code = 0;
        else
            code = 1;

        free(word_cpy);
        return code;
    }
    return 0;
}

/**
 * @brief check if the word is a reserved word
 *
 * @param word
 * @return struct token
 */
static struct token token_new(struct lexer *lexer)
{
    string_append_char(lexer->current_word, '\0');

    char *reserved_words[] = { "if",   "then",  "elif",  "else", "fi", "do",
                               "done", "while", "until", "for",  "in", "!",
                               "{",    "}",     ";",     "\n",   "|",  "&&",
                               "||",   ";;",    "\0",    "(",    ")",  "<",
                               ">",    "<<",    ">>",    "<&",   ">&", "<>",
                               ">|" };

    int family = 0;
    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(char *); i++)
    {
        // debug_printf(LOG_LEX,"test '%s' == '%s'\n", reserved_words[i],
        //              lexer->current_word->data);

        if (i == 14 || i == 23)
            family++;

        if (!strcmp(reserved_words[i], lexer->current_word->data))
        {
            return token_alloc((enum token_type)i, (enum token_family)family,
                               lexer);
        }
    }

    if (check_io_number(lexer))
        return token_alloc(TOKEN_WORD, TOKEN_FAM_IO_NUMBER, lexer);

    if (check_assignment(lexer))
        return token_alloc(TOKEN_WORD, TOKEN_FAM_ASSW, lexer);

    return token_alloc(TOKEN_WORD, TOKEN_FAM_WORD, lexer);
}

/**
 *  if (lexer->current_char == '\0')
 */
static struct token process_rule_one(struct lexer *lexer)
{
    if (lexer->current_word->len == 0)
        string_append_char(lexer->current_word, lexer->current_char);

    if (lexer->is_quoted)
    {
        lexer->is_quoted = 0;
        return token_alloc(TOKEN_ERROR, TOKEN_FAM_WORD, lexer);
    }

    return token_new(lexer);
}

/**
 *  if (lexer->last_is_op && !is_valid_operator(lexer))
 */
static struct token process_rule_three(struct lexer *lexer)
{
    lexer->last_is_op = !(lexer->current_char != '\n');

    struct token tok = token_new(lexer);

    if (is_quote(lexer))
        set_quote(lexer);
    else if (lexer->current_char != ' ' && lexer->current_char != '\t'
             && lexer->current_char != '#')
        string_append_char(lexer->current_word, lexer->current_char);

    if (lexer->current_char == '#')
        skip_comment(lexer);
    else if (first_char_op(lexer))
        lexer->last_is_op = 1;

    return tok;
}

/**
 * @brief The token recognition algorithm described in the SCL
 *
 * @param lexer
 * @return struct token
 */
static struct token parse_input_for_tok(struct lexer *lexer)
{
    if (lexer->error == 1)
    {
        lexer->error = 0;
        return token_alloc(TOKEN_ERROR, TOKEN_FAM_WORD, lexer);
    }

    lexer->current_char = io_getchar();

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 1 --------------------------------
    // -------------------------------------------------------------------------
    if (lexer->current_char == '\0')
        return process_rule_one(lexer);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 2 --------------------------------
    // -------------------------------------------------------------------------
    else if (lexer->last_is_op && !lexer->is_quoted && is_valid_operator(lexer))
        string_append_char(lexer->current_word, lexer->current_char);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 3 --------------------------------
    // -------------------------------------------------------------------------
    else if (lexer->last_is_op && !is_valid_operator(lexer))
        return process_rule_three(lexer);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 4 --------------------------------
    // -------------------------------------------------------------------------
    else if (!lexer->is_quoted && is_quote(lexer))
        set_quote(lexer);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 5 --------------------------------
    // -------------------------------------------------------------------------
    else if (!lexer->is_quoted && !lexer->is_subshell && is_subshell(lexer))
    {
        // Append '$' or '`'
        string_append_char(lexer->current_word, lexer->current_char);

        if (find_mode(lexer))
        {
            struct token tok = token_new(lexer);
            string_append_char(lexer->current_word, lexer->current_char);
            return tok;
        }
    }

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 6 --------------------------------
    // -------------------------------------------------------------------------
    else if (!lexer->is_quoted && !lexer->is_subshell && first_char_op(lexer))
    {
        lexer->last_is_op = 1;

        if (lexer->current_word->len != 0)
        {
            struct token tok = token_new(lexer);
            string_append_char(lexer->current_word, lexer->current_char);

            lexer->is_newline = lexer->current_char == '\n';

            return tok;
        }
        else
            string_append_char(lexer->current_word, lexer->current_char);
    }

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 7 --------------------------------
    // -------------------------------------------------------------------------
    else if (!lexer->is_quoted && !lexer->is_subshell
             && isblank(lexer->current_char))
    {
        if (lexer->current_word->len != 0)
            return token_new(lexer);
    }

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 8 --------------------------------
    // -------------------------------------------------------------------------
    else if (lexer->current_word->len != 0)
    {
        string_append_char(lexer->current_word, lexer->current_char);
        check_special_behavior(lexer);
    }

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 9 --------------------------------
    // -------------------------------------------------------------------------
    else if (lexer->current_char == '#')
        skip_comment(lexer);

    else
        string_append_char(lexer->current_word, lexer->current_char);

    return parse_input_for_tok(lexer);
}

struct lexer *lexer_new(int argc, char *argv[])
{
    if (io_abstraction(argc, argv) == IO_FAILED)
        return NULL;

    struct lexer *lexer = calloc(1, sizeof(struct lexer));
    lexer->last_token = token_null();
    lexer->current_word = string_create();
    return lexer;
}

struct token lexer_peek(struct lexer *lexer)
{
    if (lexer->last_token.type != TOKEN_NULL)
    {
        /*if (lexer->last_token.family == TOKEN_FAM_OPERATOR)
            lexer->last_is_op = 1;*/
        return lexer->last_token;
    }

    struct token tok;
    if (lexer->is_newline)
    {
        lexer->is_newline = 0;
        tok = (struct token){ .type = TOKEN_NEWLINE,
                              .family = TOKEN_FAM_OPERATOR,
                              .data = NULL };
        lexer->last_token = tok;
    }
    else
    {
        tok = parse_input_for_tok(lexer);
        lexer->last_token = tok;
    }

    return tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    if (lexer->last_token.type != TOKEN_NULL)
    {
        struct token tok = lexer->last_token;
        lexer->last_token = token_null();
        return tok;
    }

    return parse_input_for_tok(lexer);
}

void lexer_free(struct lexer *lexer)
{
    string_destroy(lexer->current_word);
    free(lexer);
}
