#include "lexer/lexer.h"
#include "utils/string/string.h"

void print_token (struct token *token)
{
    printf("Token: %s\n", token->data);
    
    switch (token->type)
    {
        case TOKEN_IF:
            printf("Type: TOKEN_IF\n");
            break;
        case TOKEN_THEN:
            printf("Type: TOKEN_THEN\n");
            break;
        case TOKEN_ELIF:
            printf("Type: TOKEN_ELIF\n");
            break;
        case TOKEN_ELSE:
            printf("Type: TOKEN_ELSE\n");
            break;
        case TOKEN_FI:
            printf("Type: TOKEN_FI\n");
            break;
        case TOKEN_SEMICOLONS:
            printf("Type: TOKEN_SEMICOLONS\n");
            break;
        case TOKEN_NEWLINE:
            printf("Type: TOKEN_NEWLINE\n");
            break;
        case TOKEN_WORD:
            printf("Type: TOKEN_WORD\n");
            break;
        default:
            printf("Type: TOKEN_UNKNOWN\n");
            break;
    }
}

int main(void)
{
    const char *code = "if then elif else fi ; \n";
    struct lexer *lexer = lexer_new();
}
