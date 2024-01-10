#ifndef LEXER_H
#define LEXER_H

#include "token.h"

/**
 * @brief Creates a new token
 *
 * @param type The type of the token
 * @param str The string of the token
 * @return The new token
 */
struct token new_token(enum token_type type, struct string *str);

/**
 * @brief Builds a string character by character using get_char()
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
struct token parse_input_for_tok(void);

#endif /* ! LEXER_H */
