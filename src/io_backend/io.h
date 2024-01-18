#ifndef IO_H
#define IO_H

#define IO_SUCCESS 0
#define IO_FAILED 1

#include <stddef.h>
#include "../logger/logger.h"

/**
 * @brief Transform all inputs to a unique stream.
 * @param argc Number of argument.
 * @param argv Array of arguments.
 * @return IO_SUCCESS if abstraction succeeded, IO_FAILED otherwise.
 */
int io_abstraction(int argc, char *argv[]);

/**
 * @brief Read the next character of the stream.
 * @return A character or '\0' if it reads EOF.
 */
char io_getchar(void);

/**
 * @brief Close used stream.
 * @return IO_SUCCESS if stream is closed successfuly, IO_FAILED otherwise.
 */
int io_close(void);

#endif /* ! IO_H */
