#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

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

struct token skip_comment(struct lexer *lexer)
{
    string_reset(lexer->current_word);
    while (lexer->current_char != '\n' && lexer->current_char != '\0')
        lexer->current_char = io_getchar();
    
    struct token tok;
    if (lexer->current_char == '\n')
        tok = (struct token){.type = TOKEN_NEWLINE, .family = TOKEN_FAM_OPERATOR, .data = NULL};
    else
        tok = (struct token){.type = TOKEN_EOF, .family = TOKEN_FAM_OPERATOR, .data = NULL};
    
    return tok;
}

int check_special_variable(const char *name)
{
    char special_char[] = { '$', '?', '@', '*', '#', '!', '.', '-', '+' };

    for (size_t i = 0; name[i] != '\0'; i++)
    {
        for (size_t j = 0; j < sizeof(special_char) / sizeof(char); j++)
        {
            if (name[i] == special_char[j])
                return 1;
        }
    }
    return 0;
}

int check_io_number(struct lexer *lexer)
{
    for (size_t i = 0; lexer->current_word->data[i] != '\0'; i++)
    {
        if (!isdigit(lexer->current_word->data[i]))
            return 0;
    }

    return lexer->current_char == '<' || lexer->current_char == '>';
}

int check_assignment(struct lexer *lexer)
{
    int contains_equal = 0;
    for (size_t i = 0; i < lexer->current_word->len; i++)
    {
        if (lexer->current_word->data[i] == '=')
            contains_equal++;
    }

    if (contains_equal >= 1)
    {
        if (lexer->current_word->data[0] == '=')
            return 0;

        char *word_cpy = strdup(lexer->current_word->data);
        char *variable_name = strtok(word_cpy, "=");

        int code = 0;

        if (variable_name == NULL)
            code = 0;

        else if (check_special_variable(variable_name))
            code = 0;
        else
            code = 1;

        free(word_cpy);
        return code;
    }
    return 0;
}
