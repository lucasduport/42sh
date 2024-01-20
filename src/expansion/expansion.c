#include "expansion.h"

#include <stddef.h>

static int is_special_char(char c)
{
    char special_char[] = {'?', '*', '$', '#', '@'};
    for (size_t i = 0; i < sizeof(special_char) / sizeof(char); i++)
    {
        if (special_char[i] == c)
            return 1;
    }
    return 0;
}

/**
 * @brief Check if a character is a valid character for a variable name
 *
 * @param c Character to check
 * @return int 1 if valid, 0 otherwise
 */
static int is_valid_char(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'
        || (c >= '0' && c <= '9') || is_special_char(c);
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
    return 2;
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
    // *index is at $
    char *var_name = NULL;
    size_t var_len = 0;
    size_t delim_index = *index;
    *index += 1;
    for (; (*str)[*index] != '\0';)
    {
        if (!is_valid_char((*str)[*index]))
            break;
        else
        {
            var_name = realloc(var_name, var_len + 2);
            var_len++;
            var_name[var_len - 1] = (*str)[*index];
            // Remove the character from the string
            // Acts as a shift, or as an incrementation of i
            remove_at_n(str, *index);
        }
    }
    if (var_name == NULL)
    {
        debug_printf(LOG_EXP, "[EXPANSION] bad variable name\n");
        return 1;
    }
    var_name[var_len] = '\0';
    char *var_value;
    
    if (check_env_variable(var_name))
        var_value = getenv(var_name);
    else
        var_value = get_value(env->variables, var_name);
        
    free(var_name);

    // Remove the first delimiter $
    remove_at_n(str, delim_index);

    // Insert the value of the variable in the string
    if (var_value != NULL)
    {
        insert_at_n(str, var_value, delim_index);
        *index = delim_index + strlen(var_value);
    }
    else
    {
        // If the variable is not set, remove the variable name
        *index = delim_index;
    }
    return 0;
}

/**
 * @brief Expands a brace expansion as described in the SCL
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index of the variable
 * @return int return code
 */
static int expand_brace(struct environment *env, char **str, size_t *index)
{
    // *index is at $
    size_t dollar = *index;

    // Skip the $,now at {
    *index += 1;
    size_t brace = *index;

    char *var_name = NULL;
    size_t var_len = 0;

    *index += 1;
    for (; (*str)[*index] != '\0' && (*str)[*index] != '}';)
    {
        var_name = realloc(var_name, var_len + 2);
        var_len++;
        var_name[var_len - 1] = (*str)[*index];
        // Remove the character from the string
        // Acts as a shift, or as an incrementation of i
        remove_at_n(str, *index);
    }
    if ((*str)[*index] != '}')
    {
        free(var_name);
        return 2;
    }

    var_name[var_len] = '\0';
    for (size_t i = 0; i < var_len; i++)
    {
        if (!is_valid_char(var_name[i]))
        {
            debug_printf(LOG_EXP, "[EXPANSION] bad variable name: %s\n",
                         var_name);
            free(var_name);
            return 1;
        }
    }
    char *var_value = get_value(env->variables, var_name);
    free(var_name);

    // Remove the {
    remove_at_n(str, brace);

    // Insert the value of the variable in the string
    if (var_value != NULL)
    {
        insert_at_n(str, var_value, brace);
        *index = brace + strlen(var_value);
    }
    else
    {
        // If the variable is not set, remove the variable name
        *index = brace;
    }
    // removes $ char
    remove_at_n(str, dollar);
    // removes } char
    *index -= 1;
    remove_at_n(str, *index);
    return 0;
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
    return 2;
}

char *expand_string(char *str, struct environment *env, int *ret)
{
    if (str == NULL)
        return NULL;
    
    char *copy = strdup(str);
    size_t i = 0;
    while (copy[i] != '\0')
    {
        *ret = 0;
        if (copy[i] == '\'')
            *ret = expand_single_quotes(env, &copy, &i);
        else if (copy[i] == '\"')
            *ret = expand_double_quotes(env, &copy, &i);
        else if (copy[i] == '$')
            *ret = expand_dollar(env, &copy, &i);
        else if (copy[i] == '\\')
            *ret = escape_backlash(env, &copy, &i);
        else
            i++;
        if (*ret == -1)
        {
            fprintf(stderr, "Failed to expand %s\n", str);
            free(copy);
            return NULL;
        }
    }
    return copy;
}

struct list *expansion(struct list *arguments, struct environment *env,
                       int *ret)
{
    struct list *copy = list_copy(arguments);
    struct list *p = copy;
    while (p != NULL)
    {
        char *current = p->current;
        size_t i = 0;
        while (current[i] != '\0')
        {
            *ret = 0;
            if (current[i] == '\'')
                *ret = expand_single_quotes(env, &current, &i);
            else if (current[i] == '\"')
                *ret = expand_double_quotes(env, &current, &i);
            else if (current[i] == '$')
                *ret = expand_dollar(env, &current, &i);
            else if (current[i] == '\\')
                *ret = escape_backlash(env, &current, &i);
            else
                i++;
            if (*ret != 0)
            {
                if (*ret == 2)
                    fprintf(stderr,
                            "expansion: Unexpected EOF while looking for "
                            "matching `}'\n");
                else if (*ret == 1)
                    fprintf(stderr, "expansion: Bad substitution\n");
                p->current = current;
                list_destroy(copy);
                return NULL;
            }
        }
        p->current = current;
        p = p->next;
    }
    return copy;
}
