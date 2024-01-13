#include <criterion/criterion.h>

#include "../src/io_backend/io.h"

Test(io_backend, basic)
{
    char *argv[] = { "./42sh", "-c", "echo", "Hello", "World" };
    cr_assert_eq(io_abstraction(5, argv), IO_SUCCESS);
    for (int i = 0; i < 4; i++)
        cr_assert_eq(io_getchar(), argv[2][i]);
}
