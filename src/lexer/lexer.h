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
#include "token.h"

struct lexer
{
    struct token last_token;

    struct string *current_word;
    char current_char;
    char current_quote;
    char current_subshell;

    int last_is_op;
    int is_quoted;
    int is_subshell;
    int is_newline;
    int error;
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
int is_quote(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Check if the current char may be a subshell
 *
 * @param lexer
 * @return int
 */
int is_subshell(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Update the quote state
 *
 * If the current quote is the same as the current char, the quote state is
 * updated.
 *
 * @param lexer
 */
void set_quote(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Skip all the comment char
 *
 * Skip all the comment char and set the offset to the last char.
 *
 * @param lexer
 */
void skip_comment(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Check if the lexer need to change one mode.
 *
 */
void check_special_behavior(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Get the variable name
 *
 * @param lexer
 */
void get_variable(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Find the current mode
 *
 * @param lexer
 * @return int
 */
int find_mode(struct lexer *lexer);

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
