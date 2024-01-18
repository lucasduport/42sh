#define _POSIX_C_SOURCE 200809L

#include "io.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

FILE *stream = NULL;

int io_abstraction(int argc, char *argv[])
{
    // TODO: handle arg error

    // Stdin case.
    if (argc == 1)
        stream = stdin;
    // String case.
    else if (!strcmp(argv[1], "-c"))
    {
        stream = fmemopen(argv[2], strlen(argv[2]), "r");
        if (stream == NULL)
            goto error;
    }
    // File case
    else
    {
        stream = fopen(argv[1], "r");
        if (stream == NULL)
            goto error;
    }

    return IO_SUCCESS;

error:
    debug_printf(LOG_IO_BACK, "io_abstraction: open stream failed <%d>\n",
                 errno);
    return IO_FAILED;
}

char io_getchar(void)
{
    int c = fgetc(stream);
    if (c == EOF)
        return '\0';

    return c;
}

int io_close(void)
{
    if (stream != stdin && fclose(stream) == -1)
    {
        debug_printf(LOG_IO_BACK, "io_close: close stream failed <%d>\n",
                     errno);
        return IO_FAILED;
    }

    return IO_SUCCESS;
}
