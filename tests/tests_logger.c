#include <criterion/criterion.h>

#include "../src/logger/logger.h"
#include "../src/utils/ast/ast.h"

Test(logger, basic)
{
    create_logger("stdout");
    destroy_logger();
    cr_assert_eq(1, 1);
}

Test(logger, all_logs)
{
    create_logger("stdout");
    enable_all_logs();
    disable_all_logs();
    destroy_logger();
    cr_assert_eq(1, 1);
}

Test(logger, log_type)
{
    create_logger("stdout");
    enable_log_type(LOG_AST);
    disable_log_type(LOG_BUILTIN);
    destroy_logger();
    cr_assert_eq(1, 1);
}

Test(logger, debug_printf)
{
    enable_all_logs();
    debug_printf(LOG_AST, "Hello %s\n", "World");
    destroy_logger();
    cr_assert_eq(1, 1);
}
