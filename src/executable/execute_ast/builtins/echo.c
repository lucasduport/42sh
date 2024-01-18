#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

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
    int newline = -1;
    int escape = -1;

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

    if (newline != -1)
        options->newline = newline;

    if (escape != -1)
        options->escape = escape;
    return 1;
}

/**
 * @brief Print str with escape option
 *
 * @param str String to write
 */
static void print_str_escape(char *str)
{
    size_t i = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '\\' && str[i + 1] == 'n')
        {
            putchar('\n');
            i += 2;
        }
        else if (str[i] == '\\' && str[i + 1] == 't')
        {
            putchar('\t');
            i += 2;
        }
        else
        {
            putchar(str[i]);
            i++;
        }
    }
}

int builtin_echo(struct list *list)
{
    struct list *p = list->next;

    struct option *options = calloc(1, sizeof(struct option));

    // While there is an option
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
            printf("%s", p->current);
        p = p->next;
        if (p != NULL)
            putchar(' ');
    }

    if (!options->newline)
        putchar('\n');

    free(options);
    return 0;
}
