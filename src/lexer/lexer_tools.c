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
        "&",  "&&", "(", ")",  ";",  ";;", "\n", "|",
        "||", "<",  ">", ">|", ">>", "<&", ">&", "<>"
    };

    string_append_char(lexer->current_word, lexer->current_char);

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

int is_quote(struct lexer *lexer)
{
    char quotes[] = { '\\', '\'', '\"' };
    for (size_t i = 0; i < sizeof(quotes) / sizeof(char); i++)
    {
        if (lexer->current_char == quotes[i])
            return 1;
    }
    return 0;
}

int is_subshell(struct lexer *lexer)
{
    char sub_shell_char[] = { '$', '`' };

    for (size_t i = 0; i < sizeof(sub_shell_char) / sizeof(char); i++)
    {
        if (sub_shell_char[i] == lexer->current_char)
            return 1;
    }
    return 0;
}

void set_quote(struct lexer *lexer)
{
    if (lexer->current_char == '\\')
    {
        string_append_char(lexer->current_word, lexer->current_char);
        lexer->current_char = io_getchar();
        if (lexer->current_char != '\0')
            string_append_char(lexer->current_word, lexer->current_char);
    }
    else
    {
        debug_printf(LOG_LEX, "[LEXER] set quote mode\n");
        lexer->is_quoted = 1;
        lexer->current_quote = lexer->current_char;
        string_append_char(lexer->current_word, lexer->current_char);
    }
}

void skip_comment(struct lexer *lexer)
{
    do
    {
        lexer->current_char = io_getchar();
    } while (lexer->current_char != '\n' && lexer->current_char != '\0');
}

void check_special_behavior(struct lexer *lexer)
{
    if (lexer->is_quoted && lexer->current_char == '\\')
    {
        lexer->current_char = io_getchar();
        if (lexer->current_char == '\0')
            lexer->error = 1;
        else
            string_append_char(lexer->current_word, lexer->current_char);
    }

    else if (lexer->is_quoted && lexer->current_char == lexer->current_quote)
    {
        debug_printf(LOG_LEX, "[LEXER] quit quote mode\n");
        lexer->is_quoted = 0;
    }

    else if (lexer->is_subshell && !lexer->is_quoted
             && lexer->current_char == lexer->current_subshell)
    {
        debug_printf(LOG_LEX, "[LEXER] quit subshell mode\n");
        lexer->is_subshell = 0;
    }
}

void get_variable(struct lexer *lexer)
{
    do
    {
        string_append_char(lexer->current_word, lexer->current_char);
        lexer->current_char = io_getchar();
    } while (lexer->current_char != '}' && lexer->current_char != '\0');
}

int find_mode(struct lexer *lexer)
{
    if (lexer->current_char == '`')
    {
        lexer->is_subshell = 1;
        lexer->current_subshell = '`';
        return 0;
    }
    else
    {
        lexer->current_char = io_getchar();

        if (lexer->current_char == '{')
            get_variable(lexer);
        else if (lexer->current_char == '(')
        {
            lexer->is_subshell = 1;
            lexer->current_subshell = ')';
        }
        else if (isblank(lexer->current_char))
            return 1;
        else if (first_char_op(lexer))
            return 1;

        if (lexer->current_char == '\0')
        {
            lexer->last_token = (struct token){ .type = TOKEN_EOF,
                                                .family = TOKEN_FAM_OPERATOR,
                                                .data = NULL };
            return 1;
        }

        string_append_char(lexer->current_word, lexer->current_char);
        return 0;
    }
}
