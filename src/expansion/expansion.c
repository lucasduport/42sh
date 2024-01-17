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
        if (arg->current[0] == '\'')
        {
            if (expand_single_quotes(&arg->current, env) == -1)
                return -1;
        }
        else if (arg->current[0] == '\"')
        {
            if (expand_double_quotes(&arg->current, env) == -1)
                return -1;
        }
        else
        {
            if (expand_simple(&arg->current, env) == -1)
                return -1;
        }
    }
    return 0;
}

/**
 * @brief Expand single quotes: replace any \n or ... by their respective
 * characters
 *
 * @param str String to expand
 * @param env Environment
 * @return int 0 on success, -1 on error
 */
static int expand_single_quotes(char **str, struct environment *env)
{
    for (int i = 0; (*str)[i] != '\0'; i++)
    {
        if ((*str)[i] == '\\')
        {
            switch ((*str)[i + 1])
            {
            case 'n':
                (*str)[i] = '\n';
                break;
            case 't':
                (*str)[i] = '\t';
                break;
            case 'v':
                (*str)[i] = '\v';
                break;
            case 'b':
                (*str)[i] = '\b';
                break;
            case 'r':
                (*str)[i] = '\r';
                break;
            case 'f':
                (*str)[i] = '\f';
                break;
            case 'a':
                (*str)[i] = '\a';
                break;
            case '\\':
                (*str)[i] = '\\';
                break;
            case '\'':
                (*str)[i] = '\'';
                break;
            case '\"':
                (*str)[i] = '\"';
                break;
            case '\?':
                (*str)[i] = '\?';
                break;
            default:
                debug_printf(LOG_EXPANSION,
                             "[EXPANSION] Unknown escape sequence: \\%c\n",
                             (*str)[i + 1]);
                return -1;
            }
            for (int j = i + 1; (*str)[j] != '\0'; j++)
                (*str)[j] = (*str)[j + 1];
        }
    }
}

static int expand_double_quotes(char **str, struct environment *env)
{
    // TODO: implement
}

static int expand_simple(char **str, struct environment *env)
{
    // TODO: implement
}
