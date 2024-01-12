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
    //To browse all arg list without echo at the beginning
    struct list *p = list->next;

    int option_n = 0;
    int option_e = 1;

    int end_option = 0;

    while (p != NULL && !end_option)
    {
        if (p->current[0] == '-' && !end_option)
        {
            for (int i = 1; p->current[i] != '\0'; i++)
            {
                char c = p->current[i];
                if (c != 'e' && c != 'E' && c != 'n')
                    end_option = 1;

                if (c == 'e' || c == 'E')
                    option_e = option_e == 0 ? 1 : 0;

                if (c == 'n')
                    option_n = 1;
            }
        }
        else
            end_option = 1;
        
        if (!end_option)
            p = p->next;
    }

    while (p != NULL)
    {
        if (option_e)
            printf("%s", p->current);

        else
        {
            replace(p->current);
            printf("%s", p->current);
        }

        if (!option_n)
            printf("\n");

        p = p->next;
    }
    return 0;
}
