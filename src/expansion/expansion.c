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
    if ((*str)[*index + 1] == '\n')
    {
        // Remove the \ character
        remove_at_n(str, *index);

        // Remove the \n
        remove_at_n(str, *index);
    }
    else
    {
        // Remove the \ character
        remove_at_n(str, *index);
        // Skips the next character to escape it (if not \0)
        if ((*str)[*index] != '\0')
            *index += 1;
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
        // Remove the \ character
        remove_at_n(str, *index);
        // Skip the next character to escape it
        *index += 1;
    }
    else
        *index += 1;
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
    size_t first_quote = *index;
    for (size_t i = *index + 1; (*str)[i] != '\0'; i++)
    {
        if ((*str)[i] == '\'')
        {
            // Remove the second quote
            remove_at_n(str, i);

            // Remove the first quote
            remove_at_n(str, first_quote);

            *index = i - 1;
            return 0;
        }
    }
    return -1;
}

/**
 * @brief Expands a command substitution as described in the SCL
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int 0 if success, -1 otherwise
 */
static int expand_cmd_substitution(struct environment *env, char **str,
                                   size_t *index)
{
    // TODO: implement command substitution step 3
    (void)env;
    (void)str;
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
    // *index is at $ or at {
    char *var_name = NULL;
    size_t delim_index = *index;
    size_t i;
    for (i = *index + 1; (*str)[i] != '\0';)
    {
        if (!is_valid_char((*str)[i]))
            break;
        else
        {
            var_name = realloc(var_name, i - *index + 2);
            var_name[i - *index - 1] = (*str)[i];
            // Remove the character from the string
            // Acts as a shift, or as an incrementation of i
            remove_at_n(str, i);
        }
    }

    var_name[i - *index + 1] = '\0';
    char *var_value = get_value(env->variables, var_name);
    free(var_name);

    // Insert the value of the variable in the string
    insert_at_n(str, var_value, delim_index);
    *index = *index + strlen(var_value);

    // Remove the first delimiter ($ or {)
    remove_at_n(str, delim_index);
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
    size_t dollar = *index;

    *index = *index + 1;

    expand_variable(env, str, index);

    if ((*str)[*index] == '}')
    {
        // removes $ char
        remove_at_n(str, dollar);
        // { char is remove in expand_variable
        // removes } char
        remove_at_n(str, *index);
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
        return expand_cmd_substitution(env, str, index);
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
    size_t first_quote = *index;
    *index += 1;
    for (; (*str)[*index] != '\0';)
    {
        if ((*str)[*index] == '\"')
        {
            // Remove the second quote
            remove_at_n(str, *index);

            // Remove the first quote
            remove_at_n(str, first_quote);
            *index -= 1;
            return 0;
        }
        else if ((*str)[*index] == '$')
        {
            if (expand_dollar(env, str, index) == -1)
                return -1;
        }
        else if ((*str)[*index] == '\\')
        {
            if (escape_backlash_double_quote(env, str, index) == -1)
                return -1;
        }
        else if ((*str)[*index] == '`')
        {
            if (expand_cmd_substitution(env, str, index) == -1)
                return -1;
        }
        else
            *index += 1;
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
                arg->current = current;
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
