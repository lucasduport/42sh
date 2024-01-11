#ifndef LEXER_H
#define LEXER_H

#include "../utils/string/string.h"
#include "../utils/string/string.h"
#include "../io_backend/io.h"
#include "../logger/logger.h"
#include "token.h"

struct lexer
{
    struct string *current_word;
    char *current_char;
    char *previous_char;

    int last_is_op; 
    int is_quoted;
    size_t offset;
};

/**
 * @brief Creates a new lexer
 *
 * @return The new lexer
 */
struct lexer *lexer_new(void);

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
