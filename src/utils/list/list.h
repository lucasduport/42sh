#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/**
 * @struct struct list
 * @brief Linked list structure.
 *
 */
struct list
{
    char *current; /** Current element */
    struct list *next; /** Next element */
};

/**
 * @brief Create a new linked list.
 *
 * @param current Element to add.
 * @return A new linked list on success, NULL otherwise.
 */
struct list *list_create(char *current);

/**
 * @brief Append a new element at the end of the linked list.
 *
 * @param l Linked list to modify.
 * @param current Element to append.
 */
void list_append(struct list *l, char *current);

/**
 * @brief Get the nth element.
 *
 * @param l List to search in.
 * @param n Position of the element (begin at 0).
 * @return The n-ieme element if found, NULL otherwise.
 */
char *list_get_n(struct list *l, size_t n);

/**
 * @brief Free all linked list.
 *
 * @param l Linked list to free.
 */
void list_destroy(struct list *l);

/**
 * @brief Display a linked list.
 *
 * @param l Linked list to display.
 */
void list_print(struct list *l);

#endif /* ! LIST_H */
