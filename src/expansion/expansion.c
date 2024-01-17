#include "expansion.h"

#include <stddef.h>

/**
 * @brief Check if a character is a valid character for a variable name
 *
 * @param c Character to check
 * @return int 1 if valid, 0 otherwise
 */
static int is_valid_char(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'
        || (c >= '0' && c <= '9');
}
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

/**
 * @brief Backslash acts as an escape character except when followed by a \n
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int escape_backlash(struct environment *env, char **str, size_t *index)
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
 * @brief Backslash acts as an escape character except when followed by special
 * characters (", ", $, `, \, or newline) as described in the SCL.
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int escape_backlash_double_quote(struct environment *env, char **str,
                                        size_t *index)
{
    (void)env;
    if ((*str)[*index + 1] == '$' || (*str)[*index + 1] == '`'
        || (*str)[*index + 1] == '\"' || (*str)[*index + 1] == '\\'
        || (*str)[*index + 1] == '\n')
    {
        remove_at_n(str, *index);
        *index = *index - 1;
    }
    return 0;
}

/**
 * @brief Expand single quotes: everything is taken literally except single
 * quotes
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

/**
 * @brief Expands a backquoted command as described in the SCL
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int expand_backquote(struct environment *env, char **str, size_t *index)
{
    // TODO: implement
    (void)env;
    (void)str;
    (void)(insert_at_n);
    (void)index;
    return 0;
}

/**
 * @brief Expands a variable as described in the SCL
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int expand_variable(struct environment *env, char **str, size_t *index)
{
    // *index is at $
    (void)env;
    char *var_name = NULL;
    for (size_t i = *index + 1; (*str)[i] != '\0'; i++)
    {
        if (!is_valid_char((*str)[i]))
            break;
        else
        {
            var_name = realloc(var_name, i - *index + 2);
            var_name[i - *index - 1] = (*str)[i];
        }
    }
    // FIXME: var_name is not null terminated
    /*var_name[strlen(var_name)] = '\0';
    char* var_value = get_variable(env, var_name);
    insert_at_n(str, var_value, *index);
    *index = *index + strlen(var_value);
    free(var_name);*/
    return 0;
}

/**
 * @brief Expands a brace expansion as described in the SCL
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int expand_brace(struct environment *env, char **str, size_t *index)
{
    // Skip the $,now at {
    *index = *index + 1;

    expand_variable(env, str, index);
    size_t first_brace = *index;
    if ((*str)[*index] == '}')
    {
        remove_at_n(str, first_brace);
        remove_at_n(str, *index - 1);
        *index = *index - 1;
        return 0;
    }
    else
    {
        debug_printf(LOG_EXP,
                     "[EXPANSION] expand_brace: failed to match } %s\n", *str);
        return -1;
    }
}

/**
 * @brief Redirect $ expansion to the right function
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int expand_dollar(struct environment *env, char **str, size_t *index)
{
    if ((*str)[*index + 1] == '(')
        return expand_backquote(env, str, index);
    else if ((*str)[*index + 1] == '{')
        return expand_brace(env, str, index);
    else
        return expand_variable(env, str, index);
}

/**
 * @brief Expands double quotes as described in the SCL
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int expand_double_quotes(struct environment *env, char **str,
                                size_t *index)
{
    for (size_t i = *index + 1; (*str)[i] != '\0'; i++)
    {
        if ((*str)[i] == '\"')
        {
            remove_at_n(str, *index);
            remove_at_n(str, i - 1);
            *index = i - 1;
            return 0;
        }
        else if ((*str)[i] == '$')
        {
            if (expand_dollar(env, str, &i) == -1)
                return -1;
        }
        else if ((*str)[i] == '\\')
        {
            if (escape_backlash_double_quote(env, str, &i) == -1)
                return -1;
        }
        else if ((*str)[i] == '`')
        {
            if (expand_backquote(env, str, &i) == -1)
                return -1;
        }
        else
            i++;
    }
    return -1;
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
                ret = escape_backlash(env, &current, &i);
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
