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

#define NB_TYPES 10

struct my_logs
{
    bool types_to_log[NB_TYPES];
    char *filename;
};

// If you add types, don't forget to update the NB_TYPES above
enum log_type
{
    LOG_LEX = 0,
    LOG_PARS,
    LOG_BUILTIN,
    LOG_IO_BACK,
    LOG_EXEC,
    LOG_UTILS,
    LOG_AST,
    LOG_MAIN,
    LOG_EXP,
    LOG_ENV,
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
 * @brief Enables all log types
 *
 * @return true if all log types were enabled
 * @return false if an error occured
 */
int enable_all_logs(void);

/**
 * @brief Enables a log type
 *
 * @param type the log type
 * @return true if the log type was enabled
 * @return false if an error occured
 */
int enable_log_type(enum log_type type);

/**
 * @brief Disables all log types
 *
 * @return true if all log types were disabled
 * @return false if an error occured
 */
int disable_all_logs(void);

/**
 * @brief Disables a log type
 *
 * @param type the log type
 * @return true if the log type was disabled
 * @return false if an error occured
 */
int disable_log_type(enum log_type type);

/**
 * @brief Logs a message
 *
 * @param type the message type
 * @param format the message format
 * @param ... the message arguments
 * @return true if the message was logged
 */
int debug_printf(enum log_type type, const char *format, ...);

/**
 * @brief Destroys the logger
 *
 * @return true if the logger was destroyed
 * @return false if an error occured
 */
int destroy_logger(void);

#endif /* !LOGGER_H*/
