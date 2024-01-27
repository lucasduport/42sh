#include "environment.h"

void set_number_variable(struct environment *env, struct list *param)
{
    // Recover old number of parameters
    char *past_param = get_value(env, "#");
    int nb_past = 0;
    if (past_param != NULL)
        nb_past = atoi(past_param);

    int nb_var = 0;
    // Set new_variable
    for (int i = 1; param != NULL; i++)
    {
        char var_name[20];
        sprintf(var_name, "%d", i);
        set_variable(env, var_name, param->current);
        param = param->next;
        nb_var++;
    }

    // Delete current in plus
    for (int i = nb_var + 1; i <= nb_past; i++)
    {
        char var_name[20];
        sprintf(var_name, "%d", i);
        delete_variable(&(env->variables), var_name);
    }

    // Set $# variable
    char args_count[20];
    sprintf(args_count, "%d", nb_var);
    set_variable(env, "#", args_count);
}

static char *get_from_var(struct variable *var, const char *name)
{
    while (var != NULL)
    {
        if (strcmp(var->name, name) == 0)
            return var->value;
        var = var->next;
    }
    return NULL;
}

void restore_number_variable(struct variable *past_var, struct environment *env)
{
    // Recover old number of parameters
    char *past_param = get_from_var(past_var, "#");
    int nb_past = 0;
    if (past_param != NULL)
        nb_past = atoi(past_param);

    // Recover current number of parameters
    char *curr_param = get_value(env, "#");
    int nb_curr = 0;
    if (curr_param != NULL)
        nb_curr = atoi(curr_param);

    // Delete current in plus
    for (int i = nb_past + 1; i <= nb_curr; i++)
    {
        char var_name[20];
        sprintf(var_name, "%d", i);
        delete_variable(&(env->variables), var_name);
    }

    // Restore all past variable
    for (int i = 1; i <= nb_past; i++)
    {
        char var_name[20];
        sprintf(var_name, "%d", i);
        set_variable(env, var_name, get_from_var(past_var, var_name));
    }

    set_variable(env, "#", past_param);
}

void set_exit_variable(struct environment *env, int return_code)
{
    char str[20];
    sprintf(str, "%d", return_code);
    set_variable(env, "?", str);
}

/**
 * @brief Concatenate two strings
 *
 * @param str1  The first string
 * @param str2  The second string
 */
static void string_concat(char **str1, char *str2)
{
    if (str2 == NULL)
    {
        return;
    }

    size_t len1 = strlen(*str1);
    size_t len2 = strlen(str2);
    *str1 = realloc(*str1, (len1 + len2 + 2) * sizeof(char));

    if (str1 == NULL)
    {
        exit(EXIT_FAILURE);
    }

    strcat(*str1, " ");
    strcat(*str1, str2);
}

void set_star_variable(struct environment *env)
{
    int args_count = atoi(get_value(env, "#"));
    if (args_count == 0)
        return;

    char i_str[20];
    char *star_value = strdup(get_value(env, "1"));

    int i = 2;
    while (i <= args_count)
    {
        sprintf(i_str, "%d", i);
        char *str = get_value(env, i_str);
        if (str != NULL)
            string_concat(&star_value, str);

        i++;
    }
    set_variable(env, "*", star_value);
    free(star_value);
}

struct list *get_at_variable(struct environment *env)
{
    int args_count = atoi(get_value(env, "#"));
    if (args_count == 0)
        return NULL;

    char *star_var = strdup(get_value(env, "*"));

    char *token = strtok(star_var, " ");
    struct list *list = NULL;

    while (token != NULL)
    {
        list_append(&list, strdup(token));
        token = strtok(NULL, " ");
    }

    return list;
}

void set_dollar_dollar(struct environment *env)
{
    char str[20];
    pid_t pid = getpid();
    sprintf(str, "%d", pid);
    set_variable(env, "$", str);
}

void set_uid(struct environment *env)
{
    char str[20];
    int uid = getuid();
    sprintf(str, "%d", uid);
    add_variable(&env->variables, "UID", str);
}

void set_random(struct environment *env)
{
    srand(time(NULL));
    int random_number = rand() % 32768;
    char str[20];
    sprintf(str, "%d", random_number);
    set_variable(env, "RANDOM", str);
}
