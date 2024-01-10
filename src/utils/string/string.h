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
struct string *create_string(void);

/**
 * @brief Appends a character to the string
 * 
 * @param str The string
 * @param c The character
*/
void append_char(struct string *str, char c);

/**
 * @brief Deletes a string
 * 
 * @param str The string
*/
void delete_string(struct string *str);

#endif /* ! STRING_H */
