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

#include "../io_backend/io.h"
#include "../utils/string/string.h"

static void update_single_quote(struct lexer *lexer)
{
    lexer->single_quote = lexer->single_quote == 0 ? 1 : 0;
}

struct token new_token(enum token_type type, struct string *str)
{
    struct token token;
    token.data = strdup(str->data);
    token.type = type;

    delete_string(str);

    return token;
}

/**
 * @brief check if the word is a reserved word
 *
 * @param word
 * @return struct token
 */
static struct token create_token(struct string *word)
{
    append_char(word, '\0');

    char *reserved_words[] = { "if", "then", "elif", "else",
                               "fi", ";",    "\n",   "\0" };

    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(char *); i++)
    {
        if (!strcmp(reserved_words[i], word->data))
        {
            return new_token((enum token_type)i, word);
        }
    }
    return new_token(TOKEN_WORD, word);
}

static char skip_spaces(struct lexer *lexer)
{
    char current_char = io_getchar();
    lexer->offset++;

    while (current_char == ' ')
    {
        current_char = io_getchar();
        lexer->offset++;
    }
    return current_char;
}

static char skip_comment(struct lexer *lexer)
{
    char current_char = io_getchar();
    lexer->offset++;

    while (current_char != '\n' && current_char != '\0')
    {
        current_char = io_getchar();
        lexer->offset++;
    }
    return current_char;
}

static int is_valid_char(char c)
{
    char invalid_char[] = { ' ', '\n', ';', '\0' };

    for (size_t i = 0; i < 4; i++)
    {
        if (c == invalid_char[i])
            return 0;
    }

    return 1;
}

/**
 * @brief Builds a word string character by character using io_getchar()
 * 
 * @param lexer
 * @param word
 * @param c
*/
static void feed_word(struct lexer *lexer, struct string *word, char c)
{
    while (is_valid_char(c))
    {
        printf("'%c -- %ld'\n", c, lexer->offset);
        append_char(word, c);
        c = io_getchar();
        lexer->offset++;
    }

    if (word->len == 0)
        append_char(word, c);
    else
        io_seek(--lexer->offset);
}

/**
 * @brief Builds a string character by character using io_getchar()
 *
 *  char c = get_char();
 *
 *  If c is a space -> continue
 *  If single_quote is 1 and c is a quote -> update_quote()
 *      return token word
 *  Else
 *      build the string
 *      checl if the string is a reserved word then return the token
 *
 * @return The token built from the string
 * @see new_token()
 * @see check_for_reserved_tok()
 */
static struct token parse_input_for_tok(struct lexer *lexer)
{
    struct string *word = create_string();

    char current_char = skip_spaces(lexer);
    
    if (current_char == '#')
        current_char = skip_comment(lexer);

    if (current_char == '\'')
        update_single_quote(lexer);

    if (lexer->single_quote)
    {
        while (current_char != '\'')
        {
            if (current_char == '\0')
                errx(2, "single quote is missing!\n");

            append_char(word, current_char);
            current_char = io_getchar();
            lexer->offset++;
        }
        update_single_quote(lexer);
        return new_token(TOKEN_WORD, word);
    }
    else
    {
        feed_word(lexer, word, current_char);

        return create_token(word);
    }
}

struct lexer *lexer_new(void)
{
    struct lexer *lexer = calloc(1, sizeof(struct lexer));
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
