#ifndef STRING_H
#define STRING_H

#include <unistd.h>

/**
 * @brief A basic string structure
 */
struct string
{
    char *data;
    size_t capacity;
    size_t len;
};

/**
 * @brief Creates a string
 *
 * @return The string
 */
struct string *string_create(void);

/**
 * @brief Appends a character to the string
 *
 * @param str The strings
 * @param c The character
 */
void string_append_char(struct string *str, char c);

/**
 * @brief Pop the last char to the string
 *
 */
void string_pop_char(struct string *str);

/**
 * @brief Deletes a string
 *
 * @param str The string
 */
void string_destroy(struct string *str);

/**
 * @brief duplicate a string
*/
struct string *string_dup(struct string *str);

/**
 * @brief Reset a string
 *
 * @param str The string to reset
 */
struct string *string_reset(struct string *str);

#endif /* ! STRING_H */
