#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* !_POSIX_C_SOURCE */

struct my_logs
{
    bool verbose_mode;
    bool pretty_print;
    char *filename;
};

/**
 * @brief Get the logger object
 *
 * @return the logger object
 */
struct my_logs *get_logger(void);

/**
 * @brief Opens the logger
 *
 * @return the logger file stream
 */
static FILE *open_logger(void);

/**
 * @brief Closes the logger
 *
 * @param fd the logger file stream
 */
static void close_logger(FILE *fd);

/**
 * @brief Logs a message
 *
 * @param format the message format
 * @param ... the message arguments
 */
int debug_printf(const char *format, ...);
