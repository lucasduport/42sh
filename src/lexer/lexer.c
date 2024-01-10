#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/string/string.h"

struct lexer *lexer_new(void)
{
    struct lexer *lexer = malloc(sizeof(struct lexer));
    lexer->double_quote = 0;
    lexer->single_quote = 0;
    lexer->offset = 0;

    return lexer;
}

static void update_single_quote(struct lexer *lexer)
{
    lexer->single_quote = 0 ? 1 : 0;
}

struct token new_token(enum token_type type, struct string *str)
{
    append_char(str, '\0');

    struct token token;
    token.data = strcpy(malloc(sizeof(char) * str->len), str->data);
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
static struct token check_for_reserved_tok(struct string *word)
{
    if (strcmp(word->data, "if") == 0)
        return new_token(TOKEN_IF, word);

    else if (strcmp(word->data, "then") == 0)
        return new_token(TOKEN_THEN, word);

    else if (strcmp(word->data, "elif") == 0)
        return new_token(TOKEN_ELIF, word);

    else if (strcmp(word->data, "else") == 0)
        return new_token(TOKEN_ELSE, word);

    else if (strcmp(word->data, "fi") == 0)
        return new_token(TOKEN_FI, word);

    else if (strcmp(word->data, ";") == 0)
        return new_token(TOKEN_SEMICOLONS, word);

    else if (strcmp(word->data, "\n") == 0)
        return new_token(TOKEN_NEWLINE, word);
    else
        return new_token(TOKEN_WORD, word);
}

struct token parse_input_for_tok(struct lexer *lexer)
{
    struct string *word = create_string();

    char current_char = get_char();
    lexer->offset++;

    if (current_char == ' ')
        return parse_input_for_tok(lexer);

    if (lexer->single_quote)
    {
        while (current_char != '\'')
        {
            if (current_char == '\0')
                errx(2, "a quote is missing!\n");

            append_char(word, current_char);
            current_char = get_char();
            lexer->offset++;
        }
        update_quote();
        return new_token(TOKEN_WORD, word);
    }
    else
    {
        if (current_char == '\'')
        {
            update_quote();
            return parse_input_for_tok(lexer);
        }

        if (current_char == '#')
        {
            while (current_char != '\n' && current_char != '\0')
            {
                current_char = get_char();
                lexer->offset++;
            }
        }

        while (current_char != ' ' && current_char != ';'
               && current_char != '\0')
        {
            append_char(word, current_char);
            current_char = get_char();
            lexer->offset++;
        }
        return check_for_reserved_tok(word);
    }
}

struct token lexer_peek(struct lexer *lexer)
{
    size_t offset_save = lexer->offset;
    struct token tok = parse_input_for_tok(lexer);
    size_t diff = lexer->offset - offset_save;

    lexer->offset -= diff;

    return tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    return parse_input_for_tok(lexer);
}
