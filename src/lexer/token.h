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
};

struct token
{
    enum token_type type;
    char* data;
};

#endif /* ! TOKEN_H */
