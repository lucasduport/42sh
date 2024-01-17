#include "token.h"

#include "../logger/logger.h"

void print_token(struct token token)
{
    char *token_fam[] = { "RESERVED",  "OPERATOR",     "REDIR",
                          "IO_NUMBER", "ASSIGNMENT_W", "WORD" };

    char *token_type[] = { "IF",         "THEN",      "ELIF",    "ELSE",
                           "FI",         "DO",        "DONE",    "WHILE",
                           "UNTIL",      "FOR",       "IN",      "NEG",
                           "SEMICOLONS", "NEWLINE",   "PIPE",    "AND_IF",
                           "OR_IF",      "DSEMI",     "LESS",    "GREAT",
                           "DLESS",      "DGREAT",    "LESSAND", "GREATAND",
                           "LESSGREAT",  "DLESSDASH", "CLOBBER", "EOF",
                           "AND",        "OR",        "WORD",    "NULL" };

    debug_printf(LOG_LEX,
                 "[TOKEN] Family: TOKEN_FAM_%s | Type: TOKEN_%s | Data: %s\n",
                 token_fam[token.family], token_type[token.type], token.data);
}

struct token token_null(void)
{
    struct token tok;
    tok.type = TOKEN_NULL;
    tok.family = TOKEN_FAM_WORD;
    return tok;
}

void token_free(struct token token)
{
    if (token.data != NULL)
        free(token.data);
}
