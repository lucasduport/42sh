#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_IF = 0,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_SEMICOLONS,
    TOKEN_NEWLINE,
    TOKEN_EOF,

    TOKEN_WORD,

    TOKEN_NULL,
};

struct token
{
    enum token_type type;
    char *data;
};

/**
 * @brief Print token data & token type if --verbose
 */
void print_token(struct token token);

/**
 * @brief Return a NULL token 
 */
struct token token_null();

/**
 * @brief Free the token
 */
void token_free(struct token token);

#endif /* ! TOKEN_H */
