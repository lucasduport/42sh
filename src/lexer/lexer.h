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
 * @brief Creates a new lexer
 *
 * @return The new lexer
 */
struct lexer *lexer_new(void);

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
