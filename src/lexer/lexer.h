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

/**
 * @file lexer_rules.c
 * @brief Catch the case when there is a dollar or a backslash at the top of the stack
 * 
 * @return 1 if need to call tokenizer, right after; 0 if not 
*/
int preprocess(struct lexer *lexer);

/**
 * @file lexer_rules.c
 * @brief Process rule 1
 *  EOF
*/
struct token process_rule_one(struct lexer *lexer);

/**
 * @file lexer_rules.c
 * @brief Process rule 3
 *   last_is_op and concatenation with current doesn't make an operator
*/
struct token process_rule_three(struct lexer *lexer);

/**
 * @file lexer_rules.c
 * @brief Process rule 4
 *  Quote char
*/
void process_rule_four(struct lexer *lexer);

/**
 * @file lexer_rules.c
 * @brief Process rule 5
 *  Subshell char but not in quoting mode
*/
void process_rule_five(struct lexer *lexer);

/**
 * @file lexer_rules.c
 * @brief Process rule 6
 *  current_char is first char of an operator
 * 
 * @return 1 if we need to create a new token, 0 otherwise
*/
int process_rule_six(struct lexer *lexer);



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

/**
 * @file lexer_tools.c
 * @brief ???????
*/
int check_special_variable(const char *name);

/**
 * @file lexer_tools.c
 * @brief Check if the current word could be an IO number
 */
int check_io_number(struct lexer *lexer);

/**
 * @file lexer_tools.c
 * @brief Check if the current word could be an assignment word
 */
int check_assignment(struct lexer *lexer);



/**
 * @file lexer.c
 * @brief Initialize io_abstraction and create a new lexer
 *
 * @return A new lexer or NULL if something failed
 */
struct lexer *lexer_new(int argc, char *argv[]);

/**
 * @file lexer.c
 * @brief Gets the next token without consuming it
 *
 * @return The next token
 */
struct token lexer_peek(struct lexer *lexer);

/**
 * @file lexer.c
 * @brief Gets the next token and consumes it.
 *
 * @param lexer
 * @return The next token.
 */
struct token lexer_pop(struct lexer *lexer);

/**
 * @file lexer.c
 * @brief Frees the lexer.
 *
 * @param lexer Struc lexer to free.
 */
void lexer_free(struct lexer *lexer);


/**
 * @brief Creates a new token
 *
 * @param type The type of the token
 * @param lexer
 * @return The new token
 */
struct token token_alloc(enum token_type type, enum token_family family, struct lexer *lexer);

/**
 * @brief check if the word is a reserved word
 *
 * @param word
 * @return struct token
 */
struct token token_new(struct lexer *lexer);

/**
 * @brief The token recognition algorithm described in the SCL
 *
 * @param lexer
 * @return struct token
 */
struct token tokenizer(struct lexer *lexer);

#endif /* ! LEXER_H */
