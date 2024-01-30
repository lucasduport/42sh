#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "expansion.h"

#include <err.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * @brief Check if a character is a special character
 *
 * @param c Character to check
 * @return int 1 if special, 0 otherwise
 */
static int is_special_char(char c)
{
    char special_char[] = { '?', '*', '$', '#', '@' };
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
 * @return int 0 if success, -1 otherwise    variables/variables.c \
    variables/variables.h
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
        if ((!is_valid_char((*str)[*index]) && !is_special_char((*str)[*index]))
            || (is_special_char((*str)[*index]) && var_len > 0))
            break;
        else
        {
            var_name = realloc(var_name, var_len + 2);
            var_len++;
            var_name[var_len - 1] = (*str)[*index];
            // Remove the character from the string
            // Acts as a shift, or as an incrementation of i
            remove_at_n(str, *index);
            if (var_name[var_len - 1] == '$')
                break;
        }
    }
    if (var_name == NULL)
        return 0;
    var_name[var_len] = '\0';
    char *var_value;

    if (check_env_variable(var_name))
        var_value = getenv(var_name);
    else
        var_value = get_value(env, var_name);

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
    char *var_value = get_value(env, var_name);
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
 * @brief Fork our 42sh to execute a command and insert the output in the string
 *
 * @param cmd Command to execute
 * @param current_env Environment to use
 * @return int  Return code of the command
 */
static int main_bis(char *cmd, struct environment *current_env)
{
    int argc = 3;
    char *argv[4] = { "../src/42sh", "-c", cmd, NULL };
    struct lexer *lex = lexer_new(argc, argv);
    if (lex == NULL)
    {
        if (argc > 2 && strlen(argv[2]) == 0)
            return 0;
        return 127;
    }

    // Dupplicate environment to not modify the current one
    struct environment *env = dup_environment(current_env);

    struct ast *res;
    int code = 0;
    enum parser_status parse_code = PARSER_OK;
    while (parse_code != PARSER_EOF)
    {
        parse_code = parser_input(lex, &res);
        if (parse_code == PARSER_OK || parse_code == PARSER_EOF)
        {
            if (res != NULL)
            {
                code = execute_ast(res, env);
                update_aliases(env);
                ast_free(res);
                if (env->error == STOP)
                    break;
            }
        }
        else
            code = 2;
    }

    lexer_free(lex);
    environment_free(env);
    return code;
}

/**
 * @brief Fork our 42sh to execute a command and insert the output in the string
 *
 * @param env Environment
 * @param str String to expand
 * @param index Index where we actually are in the string
 * @param cmd Command to execute
 */
static void insert_cmd_output(struct environment *env, char **str,
                              size_t *index, char *cmd)
{
    // Create a pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
        errx(1, "Failed to create pipe file descriptors.");

    int pid = fork();
    if (pid == -1)
        errx(1, "Failed to fork.");

    if (pid == 0)
    {
        // Closing read end
        close(pipe_fd[0]);
        // Redirect stdout to the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);
        int code = main_bis(cmd, env);
        close(pipe_fd[1]);
        _exit(code);
    }
    else
    {
        // Close write end
        close(pipe_fd[1]);
        char *output = NULL;
        size_t output_len = 0;
        char buffer[1024];
        ssize_t read_len;
        while ((read_len = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
        {
            output = realloc(output, output_len + read_len + 1);
            memcpy(output + output_len, buffer, read_len);
            output_len += read_len;
        }
        close(pipe_fd[0]);
        // Wait for child process to finish
        int status;
        waitpid(pid, &status, 0);
        set_exit_variable(env, status);
        if (output == NULL)
            return;
        output[output_len] = '\0';
        insert_at_n(str, output, *index);
        *index += strlen(output);
        free(output);
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
    {
        size_t dollar = *index;
        *index += 1;
        int code = expand_cmd_sub(env, str, index);
        if (env->error != NO_ERROR)
            return code;
        remove_at_n(str, dollar);
        return code;
    }

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
            if (expand_cmd_sub(env, str, index) == -1)
                return -1;
        }
        else
            *index += 1;
    }
    return 2;
}

struct ast *expand_ast(struct ast *ast, struct environment *env, int *ret)
{
    if (ast == NULL)
        return NULL;

    struct ast *copy = ast_new(ast->type);
    copy->arg = expansion(ast->arg, env, ret);
    if (*ret == -1)
    {
        ast_free(copy);
        return NULL;
    }
    copy->first_child = expand_ast(ast->first_child, env, ret);
    if (*ret == -1)
    {
        ast_free(copy);
        return NULL;
    }
    copy->is_expand = 1;
    return copy;
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
        else if (copy[i] == '`')
            *ret = expand_cmd_sub(env, &copy, &i);
        else
            i++;
        if (*ret == -1)
        {
            fprintf(stderr, "Failed expand_variable %s\n", str);
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
            else if (current[i] == '`')
                *ret = expand_cmd_sub(env, &current, &i);
            else
                i++;
            if (*ret != 0)
            {
                if (*ret == 2)
                    fprintf(stderr,
                            "expansion: Unexpected EOF while looking for "
                            "matching `%s'\n",
                            current);
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

int expand_cmd_sub(struct environment *env, char **str, size_t *index)
{
    // *index is at ( or `
    size_t first_del = *index;
    // Determine which delimiter to look for
    char del = ((*str)[*index] == '(' ? ')' : '`');
    *index += 1;
    size_t first_char_cmd = *index;
    size_t last_char_cmd = *index;
    int ret_code = 0;
    int par = 0;
    for (; (*str)[*index] != '\0';)
    {
        if ((*str)[*index] == del)
        {
            if (par == 0)
            {
                last_char_cmd = *index;
                break;
            }
            else
                par--;
        }
        else if ((*str)[*index] == '(')
            par++;
        else if ((*str)[*index] == '$')
        {
            if ((*str)[*index + 1] == '(')
            {
                size_t dollar = *index;
                *index += 1;
                int code = expand_cmd_sub(env, str, index);
                if (code != 0)
                    return code;
                remove_at_n(str, dollar);
            }
            else
                *index += 1;
        }
        else if ((*str)[*index] == '\\')
        {        return expand_brace(env, str, index);
            if (escape_backlash(env, str, index) == -1)
                return -1;
        }
        else if ((*str)[*index] == '`')
        {
            if (expand_cmd_sub(env, str, index) == -1)
                return -1;
        }
        else
            *index += 1;
    }
    if (first_char_cmd != last_char_cmd)
    {
        // Recreate command content with index
        char *cmd = calloc(last_char_cmd - first_char_cmd + 1, sizeof(char));
        strncpy(cmd, *str + first_char_cmd, last_char_cmd - first_char_cmd);
        cmd[last_char_cmd - first_char_cmd] = '\0';
        for (size_t i = first_char_cmd; i < *index; i++)
            remove_at_n(str, first_char_cmd);
        *index = first_char_cmd;
        insert_cmd_output(env, str, index, cmd);
        free(cmd);
    }

    // Remove the last delimiter
    remove_at_n(str, *index);

    // Remove the first delimiter
    remove_at_n(str, first_del);
    *index -= 1;
    if (*index != 0)
        *index -= 1;
    return ret_code;
}
