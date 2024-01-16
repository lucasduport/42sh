#include "variables.h"

#include <stdlib.h>
#include <string.h>

// Function to add a variable to the list
void set_variable(struct variable **head, const char *name, int value)
{
    struct variable *new_variable = calloc(sizeof(struct variable));
    if (new_variable == NULL) {
        debug_printf(LOG_UTILS, "")
        exit(EXIT_FAILURE);
    }

    new_variable->name = strdup(name);
    new_variable->value = value;
    new_variable->next = *head;
    *head = new_variable;
}

// Function to get the value of a variable by its name
int get_value(const struct variable *head, const char *name) {
    const struct variable *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }
        current = current->next;
    }

    // Return a default value if the variable is not found
    return 0;
}

// Function to perform a deep copy of the list of variables
struct variable *deep_copy(const struct variable *head) {
    if (head == NULL) {
        return NULL;
    }

    // Copy the first variable
    struct variable *new_head = (struct variable *)malloc(sizeof(struct variable));
    if (new_head == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    new_head->name = strdup(head->name);
    new_head->value = head->value;
    new_head->next = NULL;

    // Copy the rest of the list
    const struct variable *current = head->next;
    struct variable *new_current = new_head;
    while (current != NULL) {
        struct variable *new_variable = (struct variable *)malloc(sizeof(struct variable));
        if (new_variable == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        new_variable->name = strdup(current->name);
        new_variable->value = current->value;
        new_variable->next = NULL;

        new_current->next = new_variable;
        new_current = new_variable;

        current = current->next;
    }

    return new_head;
}

// Function to free the memory used by the list of variables
void free_variables(struct variable *head) {
    while (head != NULL) {
        struct variable *temp = head;
        head = head->next;
        free(temp->name);
        free(temp);
    }
}

// Example usage
int main() {
    struct variable *variables = NULL;

    add_variable(&variables, "x", 10);
    add_variable(&variables, "y", 20);

    printf("Value of x: %d\n", get_value(variables, "x"));
    printf("Value of y: %d\n", get_value(variables, "y"));

    struct variable *copy = deep_copy(variables);

    // Free the memory used by the lists
    free_variables(variables);
    free_variables(copy);

    return 0;
}
