#include "variables.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Add a new variable to the list
 *
 * @param head The head of the list
 * @param name The name of the variable
 * @param value The value of the variable
 * @return int 0 on success, -1 on error
 */
static int add_variable(struct variable **head, const char *name, char *value)
{
    struct variable *new_variable = calloc(1, sizeof(struct variable));
    if (new_variable == NULL)
    {
        debug_printf(LOG_UTILS,
                     "[VARIABLES] Error allocating memory for "
                     "variable [%s=%s]\n",
                     name, value);
        return -1;
    }
    new_variable->name = strdup(name);
    new_variable->value = strdup(value);
    new_variable->next = *head;
    *head = new_variable;
    return 0;
}

int set_variable(struct variable **head, const char *name, char *value)
{
    struct variable *current = *head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            free(current->value);
            current->value = strdup(value);
            return 0;
        }
        current = current->next;
    }
    if (check_env_variable(name))
    {
        int ret = 0;
        if (setenv(name, value, 1) == -1)
            ret = -1;
        return ret;
    }
    // If the variable is not found, add it to the list
    return add_variable(head, name, value);
}

char *get_value(const struct variable *head, const char *name)
{
    const struct variable *current = head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current->value;
        }
        current = current->next;
    }
    // Return NULL as a default value if the variable is not found
    return getenv(name);
}

struct variable *dup_variables(struct variable *head)
{
    struct variable *new_head = NULL;
    struct variable *current = head;
    while (current != NULL)
    {
        add_variable(&new_head, current->name, current->value);
        if (new_head == NULL)
        {
            debug_printf(LOG_UTILS,
                         "[VARIABLES] Error duplicating variable [%s=%s]\n",
                         current->name, current->value);
            return NULL;
        }
        current = current->next;
    }
    return new_head;
}

void free_variables(struct variable *head)
{
    while (head != NULL)
    {
        struct variable *temp = head;
        head = head->next;
        free(temp->name);
        free(temp->value);
        free(temp);
    }
}

void print_variables(struct variable *head)
{
    struct variable *p = head;
    while (p != NULL)
    {
        debug_printf(LOG_UTILS, "[VAR NAME] %s\n", p->name);
        debug_printf(LOG_UTILS, "[VAR VALUE] %s\n", p->value);
        debug_printf(LOG_UTILS, "next\n");
        p = p->next;
    }
}
