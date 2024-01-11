#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "io.h"

FILE *stream = NULL;

int io_abstraction(int argc, char *argv[])
{
    //TODO: handle arg error

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
        debug_printf("io_abstraction: open stream failed <%d>\n", errno);
        return IO_FAILED;
}

char io_getchar(void)
{
    int c = fgetc(stream);
    if (c == EOF)
        return '\0';

    return c;
}

int io_seek(size_t offset)
{
    if (fseek(stream, offset, SEEK_SET) == -1)
    {
        fprintf(stderr, "io_reverse: fseek failed <%d>\n", errno);
        return IO_FAILED;
    }

    return IO_SUCCESS;
}

int io_close(void)
{
    if (stream != stdin && fclose(stream) == -1)
    {
        fprintf(stderr, "io_close: close stream failed <%d>\n", errno);
        return IO_FAILED;
    }

    return IO_SUCCESS;
}
