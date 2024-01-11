#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Replace \n, \t, \\ with their respective characters
 * 
 * @param element
*/
static void replace(char *element)
{
    element = realloc(element, (strlen(element) + 1) * sizeof(char));
    for (int i = 0; element[i] != '\0'; i++)
    {
        if (element[i] == '\n')
        {

            for (int j = i; element[j] != '\0'; ++j) {
                element[j] = element[j + 1];
            }

            element[i] = '\\';
            element[i + 1] = 'n';
        }

        if (element[i] == '\\')
        {

            for (int j = i; element[j] != '\0'; ++j) {
                element[j] = element[j + 1];
            }

            element[i] = '\\';
            element[i + 1] = '\\';
        }

        if (element[i] == '\t')
        {

            for (int j = i; element[j] != '\0'; ++j) {
                element[j] = element[j + 1];
            }

            element[i] = '\\';
            element[i + 1] = 't';
        }
    }
}

int builtin_echo(struct list *list)
{
    int i = 1;
    struct list *element = list_get_n(list, i);

    int option_n = 0;
    int option_e = 1;

    int option_error = 0;

    while (element != NULL)
    {
        if (element->current[0] == '-')
        {
            for (int i = 1; element->current[i] != '\0'; i++)
            {
                char c = element->current[i];
                if (c != 'e' && c != 'E' && c != 'n')
                    option_error = 1;

                if (c == 'e' || c == 'E')
                    option_e = option_e == 0 ? 1 : 0;

                if (c == 'n')
                    option_n = 1;
            }
        }

        if (option_e)
        {
            printf("%s", element->current);
        }
        else
        {
            replace(element->current);
            printf("%s", element->current);
        }
        if (!option_n)
            printf("\n");
    }
    return 0;
}
