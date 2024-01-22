#include "token.h"

#include "../logger/logger.h"

void print_token(struct token token)
{
    debug_printf(LOG_LEX, "[LEXER] token_type %d\n", token.type);
    char *token_fam[] = { "RESERVED",  "OPERATOR",     "REDIR",
                          "IO_NUMBER", "ASSIGNMENT_W", "WORD" };

    char *token_type[] = {
        "IF",         "THEN",      "ELIF",      "ELSE",    "FI",     "DO",
        "DONE",       "WHILE",     "UNTIL",     "FOR",     "IN",     "NEG",
        "LEFT_BRACE", "RIGHT_BRACE", "SEMICOLONS", "NEWLINE",   "PIPE", "AND_IF",  
        "OR_IF",  "DSEMI", "EOF", "LEFT_PAR", "RIGHT_PAR", "LESS", "GREAT", "DLESS",   
        "DGREAT", "LESSAND", "GREATAND", "LESSGREAT", "CLOBBER", "WORD",  "NULL"
    };

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
