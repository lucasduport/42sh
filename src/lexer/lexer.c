#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif

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
static struct token token_alloc(enum token_type type, struct lexer *lexer)
{
    struct token token;
    token.data = strdup(lexer->current_word->data);
    token.type = type;

    string_reset(lexer->current_word);

    print_token(token);
    return token;
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

    char *reserved_words[] = { "if", "then", "elif", "else",
                               "fi", ";",    "\n",   "\0" };

    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(char *); i++)
    {
        if (!strcmp(reserved_words[i], lexer->current_word->data))
        {
            return token_alloc((enum token_type)i, lexer);
        }
    }

    return token_alloc(TOKEN_WORD, lexer);
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
        if (strstr(reserved_operators[i], lexer->current_word->data) != NULL)
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
    if (lexer->is_quoted == 0)
    {
        lexer->is_quoted = !lexer->is_quoted;
        lexer->current_quote = lexer->current_char;
    }

    if (lexer->current_char == lexer->current_quote)
        lexer->is_quoted = !lexer->is_quoted;

    string_append_char(lexer->current_word, lexer->current_char);
}

/**
 * @brief Get the next char
 *
 * Set the current char to the next char and increment the offset.
 *
 * @param lexer
 */
static void get_char(struct lexer *lexer)
{
    lexer->current_char = io_getchar();
    lexer->offset++;
    debug_printf("-'%c'", lexer->current_char);
}

/**
 * @brief Skip all the space char
 *
 * Skip all the space char and set the offset to the last char.
 *
 * @param lexer

static void skip_space(struct lexer *lexer)
{
    while (lexer->current_char == ' ' || lexer->current_char == '\t')
    {
        get_char(lexer);
    }
    io_seek(--lexer->offset);
}
*/

/**
 * @brief Skip all the comment char
 *
 * Skip all the comment char and set the offset to the last char.
 *
 * @param lexer
 */
static void skip_comment(struct lexer *lexer)
{
    while (lexer->current_char == '\n')
    {
        get_char(lexer);
    }
    io_seek(--lexer->offset);
}

/**
 * @brief The token recognition algorithm described in the SCL
 *
 * @param lexer
 * @return struct token
 */
static struct token parse_input_for_tok(struct lexer *lexer)
{
    get_char(lexer);

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
        // TODO: subshell completion.
    }

    // rule 6
    else if (!lexer->is_quoted && first_char_op(lexer))
    {
        lexer->last_is_op = 1;

        if (lexer->current_word->len != 0)
        {
            struct token tok = token_new(lexer);
            string_append_char(lexer->current_word, lexer->current_char);
            return tok;
        }

        string_append_char(lexer->current_word, lexer->current_char);
    }

    // rule 7
    // FIXME: May be a problem with the '\n'
    else if (!lexer->is_quoted && lexer->current_char == '\n')
        return token_new(lexer);

    // rule 8
    else if (!lexer->is_quoted && lexer->current_char == ' ')
    {
        if (lexer->current_word->len != 0)
        {
            struct token tok = token_new(lexer);
            //skip_space(lexer);
            return tok;
        }
    }

    // rule 9
    else if (lexer->current_word->len != 0)
        string_append_char(lexer->current_word, lexer->current_char);

    // rule 10
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
    lexer->current_word = string_create();
    return lexer;
}

static struct lexer *lexer_copy(struct lexer *lexer)
{
    struct lexer *copy = calloc(1, sizeof(struct lexer));
    copy->current_word = string_dup(lexer->current_word);
    copy->current_char = lexer->current_char;
    copy->current_quote = lexer->current_quote;

    copy->is_quoted = lexer->is_quoted;
    copy->last_is_op = lexer->last_is_op;
    copy->offset = lexer->offset;

    return copy;
}

struct token lexer_peek(struct lexer *lexer)
{
    struct lexer *copy = lexer_copy(lexer);
    struct token tok = parse_input_for_tok(copy);

    lexer_free(copy);

    io_seek(lexer->offset);

    print_token(tok);

    return tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    struct token res = parse_input_for_tok(lexer);
    print_token(res);
    return res;
}

void lexer_free(struct lexer *lexer)
{
    string_destroy(lexer->current_word);
    free(lexer);
}
