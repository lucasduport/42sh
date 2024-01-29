#include "lexer.h"

int first_char_op(struct lexer *lexer)
{
    char reserved_char[] = { ';', '>', '<', '|', '&', '\n', '(', ')' };
    for (size_t i = 0; i < sizeof(reserved_char) / sizeof(char); i++)
    {
        if (lexer->current_char == reserved_char[i])
            return 1;
    }
    return 0;
}

int is_valid_operator(struct lexer *lexer)
{
    char *reserved_operators[] = {
        "&",  "&&", "(", ")",  ";",  ";;", "|",
        "||", "<",  ">", ">|", ">>", "<&", ">&", "<>"
    };

    feed(lexer->current_word, lexer->current_char);

    for (size_t i = 0; i < sizeof(reserved_operators) / sizeof(char *); i++)
    {
        if (!string_n_cmp(lexer->current_word, reserved_operators[i],
                          strlen(reserved_operators[i])))
        {
            string_pop_char(lexer->current_word);
            return 1;
        }
    }
    string_pop_char(lexer->current_word);
    return 0;
}

int is_quote_char(struct lexer *lexer)
{
    char quotes[] = { '\\', '\'', '\"' };
    for (size_t i = 0; i < sizeof(quotes) / sizeof(char); i++)
    {
        if (lexer->current_char == quotes[i])
            return 1;
    }
    return 0;
}

int is_sub_char(struct lexer *lexer)
{
    char sub_shell_char[] = { '$', '`' };

    for (size_t i = 0; i < sizeof(sub_shell_char) / sizeof(char); i++)
    {
        if (sub_shell_char[i] == lexer->current_char)
            return 1;
    }
    return 0;
}

void skip_comment(struct lexer *lexer)
{
    while (lexer->current_char != '\n' && lexer->current_char != '\0')
        lexer->current_char = io_getchar();
    
    if (lexer->current_char == '\n')
        lexer->last_is_op = 1;
}
