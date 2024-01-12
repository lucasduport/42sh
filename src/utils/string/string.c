#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

#include "../logger/logger.h"

struct string *string_create(void)
{
    struct string *str = malloc(sizeof(struct string));
    str->data = malloc(sizeof(char) * 16);
    str->capacity = 16;
    str->len = 0;
    return str;
}

void string_append_char(struct string *str, char c)
{
    if (str->len + 1 == str->capacity)
    {
        str->capacity *= 2;
        str->data = realloc(str->data, str->capacity * sizeof(char));
    }
    str->data[str->len] = c;
    str->len++;
}

void string_pop_char(struct string *str)
{
    str->len--;
    str->data[str->len] = '\0';
}

void string_destroy(struct string *str)
{
    free(str->data);
    free(str);
}

struct string *string_dup(struct string *str)
{
    struct string *dup = calloc(1, sizeof(struct string));
    dup->data = malloc(sizeof(char) * str->len);
    dup->data = memcpy(dup->data, str->data, str->len);
    dup->len = str->len;
    dup->capacity = str->capacity;
    return dup;
}

struct string *string_reset(struct string *str)
{
    string_destroy(str);
    return string_create();
}
