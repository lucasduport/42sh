#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct option
{
    int newline;
    int escape;
};

/**
 *  @brief Parse an option and check if is valid. 
 *
 *  @param str  String containing the option to parse.
 *  @param options Option structure that save set options.
 *  @return 1 if the parsed option is valid, 0 otherwise.
 */
static int parse_option(char *str, struct option *options)
{
    int newline = 0;
    int escape = 0;

    for (size_t i = 1; str[i] != '\0'; i++)
    {
        char c = str[i];
        if (c == 'e')
            escape = 1;

        else if (c == 'E')
            escape = 0;

        else if (c == 'n')
            newline = 1;

        else
            return 0;
    }

    options->newline = newline;
    options->escape = escape;
    return 1;
}

/**
 * @brief Print special characters '\n' '\t' '\\' litteraly
 *
 * example : '\n' is interpreted as \ and n separated.
 *
 * @param c the char to proint
*/
static void print_special_char_all(char c)
{
    if (c == '\n')
    {
        putchar('\\');
        putchar('n');
    }
    else if (c == '\t')
    {
        putchar('\\');
        putchar('t');
    }
    else if (c == '\\')
        putchar('\\');
    else
        putchar(c);
}

/**
 * @brief Print str with no escape option.
 * 
 * With quote -> exact same output.
 * 
 * Without quote -> one BS => pass
 *               -> double BS => pass
 * 
 * @param str String to write
 */
static void print_str(char *str)
{
    size_t i = 0;
    int is_quot = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '\'')
            is_quot = !is_quot;
        
        else if (is_quot)
            print_special_char_all(str[i]);
        
        else
        {
            if (str[i] == '\\')
            {
                if (str[i + 1] == '\\')
                {
                    putchar('\\');
                    i++;
                }
            }
            else
                putchar(str[i]);
        }
        i++;
    }
}

/**
 * @brief Print str with escape option
 *
 * @param str String to write
 */
static void print_str_escape(char *str)
{
    size_t i = 0;
    int is_quot = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '\'')
            is_quot = !is_quot;
        
        else if (is_quot)
            putchar(str[i]);
        
        else
        {
            if (str[i] != '\\')
                putchar(str[i]);
        }
        i++;
    }
}

int builtin_echo(struct list *list)
{
    struct list *p = list->next;

    struct option *options = calloc(1, sizeof(struct option));

    //While there is an option
    while (p != NULL && p->current[0] == '-')
    {
        if (!parse_option(p->current, options))
            break;

        p = p->next;
    }

    while (p != NULL)
    {
        if (options->escape)
            print_str_escape(p->current);
        else
            print_str(p->current);
        p = p->next;
    }

    if (!options->newline)
        putchar('\n');
    return 0;
}
