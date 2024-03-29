#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    // -------- RESERVED WORDS --------
    TOKEN_IF = 0,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_DO,
    TOKEN_DONE,

    TOKEN_WHILE,
    TOKEN_UNTIL,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_NEG,
    TOKEN_CASE,
    TOKEN_ESAC,

    TOKEN_LEFT_BRACE, // {
    TOKEN_RIGHT_BRACE, // }

    // ------- CONTROL OPERATORS -------
    TOKEN_SEMICOLONS, // ;
    TOKEN_NEWLINE, // \n
    TOKEN_PIPE, // |
    TOKEN_AND_IF, // &&
    TOKEN_OR_IF, // ||
    TOKEN_DSEMI, // ;;
    TOKEN_EOF,

    TOKEN_LEFT_PAR, // (
    TOKEN_RIGHT_PAR, // )

    // ---- REDIRECTION OPERATORS ------
    TOKEN_LESS, // <
    TOKEN_GREAT, // >
    TOKEN_DLESS, // <<
    TOKEN_DGREAT, // >>
    TOKEN_LESSAND, // <&
    TOKEN_GREATAND, // >&
    TOKEN_LESSGREAT, // <>
    TOKEN_CLOBBER, // >|

    // ---------------------------------
    TOKEN_WORD,

    TOKEN_NULL,
    TOKEN_ERROR
};

enum token_family
{
    TOKEN_FAM_RESERVED,
    TOKEN_FAM_OPERATOR,
    TOKEN_FAM_REDIR,
    TOKEN_FAM_IO_NUMBER,
    TOKEN_FAM_ASSW,
    TOKEN_FAM_WORD,
};

struct token
{
    enum token_type type;
    enum token_family family;
    char *data;
};

/**
 * @brief Return a NULL token
 */
struct token token_null(void);

/**
 * @brief Free the token
 */
void token_free(struct token token);

#endif /* ! TOKEN_H */
