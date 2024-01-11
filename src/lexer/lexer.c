#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
static struct token token_new(struct string *word)
{
    append_char(word, '\0');

    char *reserved_words[] = { "if", "then", "elif", "else",
                               "fi", ";",    "\n",   "\0" };

    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(char *); i++)
    {
        if (!strcmp(reserved_words[i], word->data))
        {
            return token_alloc((enum token_type)i, word);
        }
    }

    return token_alloc(TOKEN_WORD, word);
}

static int first_char_op(char c)
{
    char reserved_char[] = { ';', '>', '<', '|', '&', '\n', '(', ')' };
    for (size_t i = 0; i < sizeof(reserved_char) / sizeof(char); i++)
    {
        if (c == reserved_char[i])
            return 1;
    }
    return 0;
}

static int is_valid_operator(struct lexer *lexer)
{
    char reserved_operators[] = { '&',  '&&', '(',  ')',  ';',   ';;',
                                  '\n', '|',  '||', '<',  '>',   '>|',
                                  '<<', '>>', '<&', '&>', '<<-', '<>' };

    string_append_char(lexer->current_word, lexer->current_char);
    for (size_t i = 0; i < sizeof(reserved_operators) / sizeof(char); i++)
    {
        if (strstr(reserved_operators[i], lexer->current_word) != NULL)
        {
            string_pop_char(lexer->current_word);
            return 1;
        }
    }
    string_pop_char(lexer->current_word);
    return 0;
}

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

static void update_quote(struct lexer *lexer)
{
    lexer->is_quoted = lexer->is_quoted == 0 ? 1 : 0;
}

static void get_char(struct lexer *lexer)
{
    lexer->current_char = io_getchar();
    lexer->offset++;
}

static void skip_space(struct lexer *lexer)
{
    get_char(lexer);
    while (lexer->current_char == ' ' || lexer->current_char == '\t')
    {
        get_char(lexer);
    }
}

static void skip_comment(struct lexer *lexer)
{
    get_char(lexer);
    while (lexer->current_char == '\n')
    {
        get_char(lexer);
    }
}

static struct token parse_input_for_tok(struct lexer *lexer)
{
    get_char(lexer);

    if (lexer->current_char == '\0')
    {
        if (lexer->current_word->len == 0)
            string_append_char(lexer->current_word, lexer->current_char);

        return token_new(lexer->current_word);
    }

    else if (lexer->last_is_op && !lexer->is_quoted && is_valid_operator(lexer))
        string_append_char(lexer->current_word, lexer->current_char);

    else if (lexer->last_is_op && !is_valid_operator(lexer))
        return token_new(lexer->current_word);

    else if (is_quote(lexer->current_char && !lexer->is_quoted))
        update_quote(lexer);

    else if (!lexer->is_quoted && is_subshell(lexer))
    {
        // TODO: subshell completion.
    }

    else if (!lexer->is_quoted && first_char_op(lexer->current_char))
    {
        struct token tok = token_new(lexer->current_word);
        string_append_char(lexer->current_word, lexer->current_char);
        return tok;
    }
    else if (!lexer->is_quoted && lexer->current_char == '\n')
        return token_new(lexer->current_word);

    else if (!lexer->is_quoted && lexer->current_char == ' ')
    {
        struct token tok = token_new(lexer->current_word);
        skip_space(lexer);
        return tok;
    }

    else if (lexer->current_word->len != 0)
        string_append_char(lexer->current_word, lexer->current_char);

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
<<<<<<< .merge_file_6hfCWL
    lexer->current_word = string_create();
=======
  
>>>>>>> .merge_file_Co2ik4
    return lexer;
}

struct token lexer_peek(struct lexer *lexer)
{
    size_t offset_save = lexer->offset;

    struct token tok = parse_input_for_tok(lexer);
    io_seek(offset_save);

    lexer->offset = offset_save;

    return tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    return parse_input_for_tok(lexer);
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}
