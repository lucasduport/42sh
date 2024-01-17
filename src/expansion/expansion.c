#include "expansion.h"

#include <stddef.h>

/**
 * @brief Expand a variable
 *
 * @param str String to expand
 * @param to_insert String to insert
 * @param n Index where to insert
 */
static void insert_at_n(char **str, char *to_insert, size_t n)
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
 * @brief Remove the nth character from a string
 *
 * @param str String to remove from
 * @param int Index of the character to remove
 */
static void remove_at_n(char **str, size_t n)
{
    char *new_str = calloc(strlen(*str), sizeof(char));
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


static int expand_backlash(struct environment *env, char **str, size_t *index)
{
    (void)env;
    remove_at_n(str, *index);
    if ((*str)[*index] == '\n')
    {
        remove_at_n(str, *index);
        *index = *index - 1;
    }
    return 0;
}
/**
 * @brief Expand single quotes: does not expand anything
 *
 * @param str String to expand
 * @param env Environment
 * @return int index of the character after the last quote, NULL if no
 */
static int expand_single_quotes(struct environment *env, char **str,
                                size_t *index)
{
    (void)env;
    for (size_t i = *index + 1; (*str)[i] != '\0'; i++)
    {
        if ((*str)[i] == '\'')
        {
            remove_at_n(str, *index);
            remove_at_n(str, i - 1);
            *index = i - 1;
            return 0;
        }
    }
    return -1;
}

static int expand_double_quotes(struct environment *env, char **str,
                                size_t *index)
{
    // TODO: implement
    (void)env;
    (void)insert_at_n;
    (void)str;
    (void)index;
    return 0;
}

static int expand_dollar(struct environment *env, char **str, size_t *index)
{
    // TODO: implement
    (void)env;
    (void)str;
    (void)index;
    return 0;
}

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
        size_t i = 0;
        while (current[i] != '\0')
        {
            int ret = 0;
            if (current[i] == '\'')
                ret = expand_single_quotes(env, &current, &i);
            else if (current[i] == '\"')
                ret = expand_double_quotes(env, &current, &i);
            else if (current[i] == '$')
                ret = expand_dollar(env, &current, &i);
            else if (current[i] == '\\')
                ret = expand_backlash(env, &current, &i);
            else
                i++;
            if (ret == -1)
            {
                debug_printf(LOG_EXP,
                             "[EXPANSION] expansion: failed to expand %s\n",
                             current);
                return -1;
            }
        }
        arg->current = current;
        arg = arg->next;
    }
    return 0;
}
