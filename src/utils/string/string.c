#include "string.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct string *create_string(void)
{
    struct string *str = malloc(sizeof(struct string));
    str->data = malloc(sizeof(char) * 16);
    str->capacity = 16;
    str->len = 0;
    return str;
}

void append_char(struct string *str, char c)
{
    if (str->len == str->capacity)
    {
        str->capacity *= 2;
        str->data = realloc(str->data, str->capacity * sizeof(char));
    }
    str->data[str->len] = c;
    str->len++;
}

void delete_string(struct string *str)
{
    free(str->data);
    free(str);
}
