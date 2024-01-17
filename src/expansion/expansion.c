#include "expansion.h"

/**
 * @brief Try every expansions on the arguments
 *
 * @param arguments Arguments to expand
 * @param env Environment
 */
int expansion(struct list *arguments, struct environment *env)
{
    struct list *arg = arguments;
    while (arg != NULL)
    {
        char *current = arg->current;
        while (current != '\0')
        {
            char *next = NULL;

            if (current == '\'')
                next = expand_single_quotes(&current, env);
            else if (current == '\"')
                next = expand_double_quotes(&current, env);
            else if (current == '$')
                next = expand_dollar(&current, env);

            // Check if an error occured
            if (next == NULL)
                return -1;
            current = next;
        }
    }
    return 0;
}

/**
 * @brief Expand single quotes: does not expand anything
 *
 * @param str String to expand
 * @param env Environment
 * @return char * index of the character after the last quote, NULL if no
 */
static char *expand_single_quotes(char **str, struct environment *env)
{
    char *first_quote = *str;
    for (int i = 1; (*str)[i] != '\0'; i++)
    {
        if ((*str)[i] == '\'')
        {
            remove_at_n(str, first_quote);
            remove_at_n(str, i - 1);
            return *str + i - 1;
        }
    }
    return NULL;
}

static int expand_double_quotes(char **str, struct environment *env)
{
    // TODO: implement
}

static int expand_dollar(char **str, struct environment *env)
{
    // TODO: implement
}

/**
 * @brief Expand a variable
 *
 * @param str String to expand
 * @param to_insert String to insert
 * @param n Index where to insert
 */
static void insert_at_n(char **str, char *to_insert, int n)
{
    char *new_str = calloc(strlen(*str) + strlen(to_insert) + 1, sizeof(char));
    if (new_str == NULL)
    {
        debug_printf(LOG_EXP,
                     "[EXPANSION] insert_at_n: new_str allocation failed\n");
        return;
    }
    strncpy(new_str, *str, n);
    strcat(new_str, to_insert);
    strcat(new_str, *str + n);
    free(*str);
    *str = new_str;
}

/**
 * @brief Remove a character at a given index. The string is reallocated.
 *
 *
 * @param str String to remove from
 * @param n Index to remove
 */
static void remove_at_n(char **str, int n)
{
    char *new_str = calloc(strlen(*str) + 1, sizeof(char));
    if (new_str == NULL)
    {
        debug_printf(LOG_EXP,
                     "[EXPANSION] remove_at_n: new_str allocation failed\n");
        return;
    }
    strncpy(new_str, *str, n);
    strcat(new_str, *str + n + 1);
    free(*str);
    *str = new_str;
}
