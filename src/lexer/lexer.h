#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../io_backend/io.h"
#include "../logger/logger.h"
#include "../utils/string/string.h"
#include "../utils/stack/stack.h"
#include "token.h"

struct lexer
{
    struct token last_token;

    struct string *current_word;
    char current_char;

    struct stack *mode_stack;

    int last_is_op;
};

// --------------------------- LEXER TOOLS ------------------------------------

/**
 * @file lexer_tools.c
 * @brief Check if the first char is an operator
 *
 * @param c
 * @return int
 */
int first_char_op(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Check if the current char can form a valid operator with the previous
 * char
 *
 * (ex: '>' and '>' can form '>>')
 *
 * @param lexer
 * @return int
 */
int is_valid_operator(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Check if the current char is a quote
 *
 * @param lexer
 * @return int
 */
int is_quote_char(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Check if the current char may be a subshell
 *
 * @param lexer
 * @return int
 */
int is_sub_char(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Skip all the comment char
 *
 * Skip all the comment char and set the offset to the last char.
 *
 * @param lexer
 */
struct token skip_comment(struct lexer *lexer);

//
//
//
//
// ------------------------- LEXER FUNCTIONS ----------------------------------

/**
 * @brief Initialize io_abstraction and create a new lexer
 *
 * @return A new lexer or NULL if something failed
 */
struct lexer *lexer_new(int argc, char *argv[]);

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
