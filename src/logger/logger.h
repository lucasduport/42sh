#ifndef LOGGER_H
#define LOGGER_H

#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* !_POSIX_C_SOURCE */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct my_logs
{
    bool verbose_mode;
    bool pretty_print;
    char *filename;
};

/**
 * @brief Creates a logger
 *
 * @param filename the logger filename
 * @return true if the logger was created
 * @return false if an error occured
 */
int create_logger(char *filename);

/**
 * @brief Logs a message
 *
 * @param format the message format
 * @param ... the message arguments
 * @return true if the message was logged
 */
int debug_printf(const char *format, ...);

/**
 * @brief Destroys the logger
 *
 * @return true if the logger was destroyed
 * @return false if an error occured
 */
int destroy_logger(void);

#endif /* !LOGGER_H*/
