#include <criterion/criterion.h>

#include "../src/io_backend/io.h"

Test(io_backend, basic)
{
    char *argv[] = { "./42sh", "-c", "echo", "Hello", "World" };
    io_abstraction(5, argv);
    for (int i = 0; i < 4; i++)
        io_getchar();
    io_close();
    cr_assert_eq(1, 1);
}

Test(io_backend, basic2)
{
    char *argv[] = { "./42sh", "-c", "echo"};
    io_abstraction(3, argv);
    for (int i = 0; i < 4; i++)
        io_getchar();
    io_close();
    cr_assert_eq(1, 1);
}

Test(io_backend, basic3)
{
    char *argv[] = { "./42sh"};
    io_abstraction(1, argv);
    io_close();
    cr_assert_eq(1, 1);
}

Test(io_backend, basic4)
{
    char *argv[] = { "./42sh", "script.sh"};
    io_abstraction(2, argv);
    cr_assert_eq(1, 1);
}
