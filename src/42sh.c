#include "src/logger/logger.h"

int main(int argc, char **argv)
{
    create_logger("stdout");
    debug_printf("Hello World!");
    (void)argc;
    (void)argv;
    return (0);
}
