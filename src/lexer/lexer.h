#ifndef LEXER_H
#define LEXER_H

#include "../utils/string/string.h"
#include "token.h"

struct lexer
{
    int single_quote;
    int double_quote;
    size_t offset;
};

/**
 * @brief Initialize io_abstraction and create a new lexer
 *
 * @return A new lexer or NULL if something failed
 */
struct lexer *lexer_new(int argc, char *argv[]);

/**
 * @brief Creates a new token
 *
 * @param type The type of the token
 * @param str The string of the token
 * @return The new token
 */
struct token new_token(enum token_type type, struct string *str);

/**
 * @brief Gets the next token without consuming it
 *
 * @return The next token
 */
struct token lexer_peek(struct lexer *lexer);

/**
 * @brief Gets the next token and consumes it.
 *
 * @param lexer
 * @return The next token.
 */
struct token lexer_pop(struct lexer *lexer);

/**
 * @brief Frees the lexer.
 *
 * @param lexer Struc lexer to free.
 */
void lexer_free(struct lexer *lexer);

#endif /* ! LEXER_H */
