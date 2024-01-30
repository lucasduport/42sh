#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../../lexer/lexer.h"
#include "../../../parser/parser.h"
#include "../execute.h"

/**
 * @brief Parse and execute a file
 *
 * @param file The file to parse and execute
 * @param current_env The current environment
 */
static int parse_and_exec(char *file, struct environment *current_env)
{
    char *argv[] = { "42sh", file, NULL };
    int argc = 2;

    // Initialise lexer
    struct lexer *lex = lexer_new(argc, argv);
    if (lex == NULL)
    {
        if (argc > 2 && strlen(argv[2]) == 0)
            return 0;

        debug_printf(LOG_UTILS, "[DOT] Failed initialize lexer\n");
        return 127;
    }

    struct environment *env = environment_new();
    if (env == NULL)
    {
        debug_printf(LOG_UTILS, "[DOT] Failed initialize env\n");
        return 2;
    }

    set_environment(env, argc, argv);

    // Initialise variable used for parsing
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
                // ast_print(res);
                debug_printf(LOG_AST, "\n");
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

    struct variable *vars = env->variables;
    while (vars != NULL)
    {
        if (strcmp(vars->name, "UID") != 0)
            set_variable(current_env, vars->name, vars->value);
        vars = vars->next;
    }

    struct function *funcs = env->functions;
    while (funcs != NULL)
    {
        set_function(current_env, funcs->name, funcs->body);
        funcs = funcs->next;
    }

    lexer_free(lex);
    environment_free(env);
    return code;
}

/**
 * @brief Check if the file name contains a slash
 *
 * @param name The file name
 * @return int 1 if the file name contains a slash, 0 otherwise
 */
static int check_slash(char *name)
{
    size_t i = 0;
    while (name[i] != '\0')
    {
        if (name[i] == '/')
            return 1;
        i++;
    }
    return 0;
}

int builtin_dot(struct list *list, struct environment *env)
{
    if (list == NULL || list->next == NULL)
    {
        fprintf(stderr, "42sh: .: filename argument required");
        return set_error(env, STOP, 2);
    }

    char *file = list->next->current;

    int code = 0;

    struct stat sb;
    if (stat(file, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        fprintf(stderr, "42sh: .: %s: is a directory\n", file);
        return set_error(env, STOP, 1);
    }
    else if (check_slash(file))
    {
        code = parse_and_exec(file, env);
        if (code == 127 || code == 2)
        {
            fprintf(stderr, "42sh: .: %s: file not found\n", file);
            return set_error(env, STOP, (code == 127 ? 1 : 2));
        }
        return code;
    }
    else
    {
        int parse = 0;
        char *path_copy = getenv("PATH");
        char *path_token = strtok(path_copy, ":");
        while (path_token != NULL)
        {
            char *file_path =
                calloc(strlen(path_token) + strlen(file) + 2, sizeof(char));
            strcpy(file_path, path_token);
            strcat(file_path, "/");
            strcat(file_path, file);

            struct stat sb;
            if (stat(file_path, &sb) == 0 && sb.st_mode & S_IRUSR)
            {
                code = parse_and_exec(file_path, env);
                free(file_path);
                parse = 1;
                if (code == 127 || code == 2)
                {
                    fprintf(stderr, "42sh: .: %s: file not found\n", file);
                    return set_error(env, STOP, (code == 127 ? 1 : 2));
                }
                break;
            }
            free(file_path);
            path_token = strtok(NULL, ":");
        }
        if (parse == 0)
        {
            fprintf(stderr, "42sh: .: %s: file not found\n", file);
            return set_error(env, STOP, 1);
        }
        return code;
    }
}
