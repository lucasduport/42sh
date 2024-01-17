#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../logger/logger.h"

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

static int check_io_number(struct lexer *lexer)
{
    for (size_t i = 0; i < lexer->current_word->len; i++)
    {
        if (!isdigit(lexer->current_word->data[i]))
            return 0;
    }

    return lexer->current_char == '<' || lexer->current_char == '>';
}

static int check_assignment(struct lexer *lexer)
{
    size_t len = lexer->current_word->len;
    if (lexer->current_word->data[0] == '='
        || lexer->current_word->data[len - 1] == '=')
        return 0;

    int contains_equal = 0;
    for (size_t i = 0; i < lexer->current_word->len; i++)
    {
        if (lexer->current_word->data[i] == '=')
            contains_equal++;
    }

    return contains_equal == 1;
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
                               ";",    "\n",    "|",     "&&",   "||", ";;",
                               "<",    ">",     "<<",    ">>",   "<&", "&>",
                               "<>",   "<<-",   ">|",    "\0" };

    int family = 0;
    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(char *); i++)
    {
        // debug_printf(LOG_LEX,"test '%s' == '%s'\n", reserved_words[i],
        //              lexer->current_word->data);

        if (i == 12 || i == 18)
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
        return token_alloc(TOKEN_WORD, TOKEN_FAM_ASSIGNMENT_W, lexer);

    return token_alloc(TOKEN_WORD, TOKEN_FAM_WORD, lexer);
}

/**
 * @brief Check if the first char is an operator
 *
 * @param c
 * @return int
 */
static int first_char_op(struct lexer *lexer)
{
    char reserved_char[] = { ';', '>', '<', '|', '&', '\n', '(', ')' };
    for (size_t i = 0; i < sizeof(reserved_char) / sizeof(char); i++)
    {
        if (lexer->current_char == reserved_char[i])
            return 1;
    }
    return 0;
}

/**
 * @brief Check if the current char can form a valid operator with the previous
 * char
 *
 * (ex: '>' and '>' can form '>>')
 *
 * @param lexer
 * @return int
 */
static int is_valid_operator(struct lexer *lexer)
{
    char *reserved_operators[] = { "&",  "&&", "(",  ")",  ";",   ";;",
                                   "\n", "|",  "||", "<",  ">",   ">|",
                                   "<<", ">>", "<&", "&>", "<<-", "<>" };

    string_append_char(lexer->current_word, lexer->current_char);

    for (size_t i = 0; i < sizeof(reserved_operators) / sizeof(char *); i++)
    {
        // FIXME: May be a probleme with the last null char
        if (!string_n_cmp(lexer->current_word, reserved_operators[i],
                          strlen(reserved_operators[i])))
        {
            string_pop_char(lexer->current_word);
            return 1;
        }
    }
    string_pop_char(lexer->current_word);
    return 0;
}

/**
 * @brief Check if the current char is a quote
 *
 * @param lexer
 * @return int
 */
static int is_quote(struct lexer *lexer)
{
    char quotes[] = { '\\', '\'', '\"' };
    for (size_t i = 0; i < sizeof(quotes) / sizeof(char); i++)
    {
        if (lexer->current_char == quotes[i])
            return 1;
    }
    return 0;
}

/**
 * @brief Check if the current char may be a subshell
 *
 * @param lexer
 * @return int
 */
static int is_subshell(struct lexer *lexer)
{
    char sub_shell_char[] = { '$', '`' };

    for (size_t i = 0; i < sizeof(sub_shell_char) / sizeof(char); i++)
    {
        if (sub_shell_char[i] == lexer->current_char)
            return 1;
    }
    return 0;
}

/*
static int test_end_expansion(struct lexer *lexer)
{
    if (lexer->current_expansion == '{')
        return lexer->current_char == '}';

    else if (lexer->current_expansion == '(')
        return lexer->current_char == ')';

    else
        return lexer->current_char == '`';
}

static void feed_until_end(struct lexer *lexer)
{
    do
    {
        lexer->current_char = io_getchar();

        if (lexer->current_char == lexer->current_expansion)
            lexer->count_expansion++;

        if (test_end_expansion(lexer))
            lexer->count_expansion--;

        string_append_char(lexer->current_word, lexer->current_char);

    } while (!test_end_expansion(lexer) && lexer->count_expansion != 0);
}


static void feed_expansion(struct lexer *lexer)
{
    if (lexer->current_char == '`')
    {
        lexer->current_expansion = lexer->current_char;
        lexer->count_expansion++;
        feed_until_end(lexer);
    }

    else if (lexer->current_char == '$')
    {
        string_append_char(lexer->current_word, lexer->current_char);

        lexer->current_char = io_getchar();

        if (lexer->current_char == '{' || lexer->current_char == '(')
        {
            string_append_char(lexer->current_word, lexer->current_char);
            lexer->current_expansion = lexer->current_char;
            lexer->count_expansion++;
            feed_until_end(lexer);
        }
        // "bonjour $name ! comment vas tu ?"
    }
}*/

/**
 * @brief Update the quote state
 *
 * If the current quote is the same as the current char, the quote state is
 * updated.
 *
 * @param lexer
 */
static void update_quote(struct lexer *lexer)
{
    debug_printf(LOG_LEX, "update_quote\n");
    if (lexer->current_char == '\\')
    {
        string_append_char(lexer->current_word, lexer->current_char);
        lexer->current_char = io_getchar();
        debug_printf(LOG_LEX, "current_char: %c\n", lexer->current_char);
        string_append_char(lexer->current_word, lexer->current_char);
    }
    else
    {
        if (lexer->is_quoted == 0)
        {
            debug_printf(LOG_LEX, "enter quote mode\n");
            lexer->is_quoted = !lexer->is_quoted;
            lexer->current_quote = lexer->current_char;
        }

        else if (lexer->current_char == lexer->current_quote)
        {
            debug_printf(LOG_LEX, "quit quote mode\n");
            lexer->is_quoted = !lexer->is_quoted;
        }

        string_append_char(lexer->current_word, lexer->current_char);
    }
}

/**
 * @brief Skip all the comment char
 *
 * Skip all the comment char and set the offset to the last char.
 *
 * @param lexer
 */
static void skip_comment(struct lexer *lexer)
{
    while (lexer->current_char != '\n')
        lexer->current_char = io_getchar();
}

/**
 * @brief The token recognition algorithm described in the SCL
 *
 * @param lexer
 * @return struct token
 */
static struct token parse_input_for_tok(struct lexer *lexer)
{
    lexer->current_char = io_getchar();
    debug_printf(LOG_LEX, "%c\n", lexer->current_char);

    // rule 1
    if (lexer->current_char == '\0')
    {
        if (lexer->current_word->len == 0)
            string_append_char(lexer->current_word, lexer->current_char);

        return token_new(lexer);
    }

    // rule 2
    else if (lexer->last_is_op && !lexer->is_quoted && is_valid_operator(lexer))
        string_append_char(lexer->current_word, lexer->current_char);

    // rule 3
    else if (lexer->last_is_op && !is_valid_operator(lexer))
    {
        lexer->last_is_op = 0;
        struct token tok = token_new(lexer);

        if (lexer->current_char != ' ' && lexer->current_char != '\t')
            string_append_char(lexer->current_word, lexer->current_char);

        return tok;
    }

    // rule 4
    else if (is_quote(lexer))
        update_quote(lexer);

    // rule 5
    else if (!lexer->is_quoted && is_subshell(lexer))
    {
        // debug_printf(LOG_LEX, "EXPANSION!\n");
        // feed_expansion(lexer);
        string_append_char(lexer->current_word, lexer->current_char);
    }

    // rule 6
    else if (!lexer->is_quoted && first_char_op(lexer))
    {
        lexer->last_is_op = 1;

        if (lexer->current_word->len != 0)
        {
            struct token tok = token_new(lexer);
            string_append_char(lexer->current_word, lexer->current_char);

            if (lexer->current_char == '\n')
                lexer->is_newline = 1;

            return tok;
        }

        string_append_char(lexer->current_word, lexer->current_char);
    }

    // rule 7
    else if (!lexer->is_quoted && isblank(lexer->current_char))
    {
        if (lexer->current_word->len != 0)
            return token_new(lexer);
    }

    // rule 8
    else if (lexer->current_word->len != 0)
        string_append_char(lexer->current_word, lexer->current_char);

    // rule 9
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
        return lexer->last_token;

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
