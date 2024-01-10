#ifndef IO_BACKEND_H
#define IO_BACKEND_H

#define IO_SUCCESS 0
#define IO_FAILED 1

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
 * @brief Change the current position of the file pointer.
 * @param offset Position to set.
 * @return A character or '\0' if it reads EOF.
 */
int io_seek(size_t offset);

/**
 * @brief Close used stream.
 * @return IO_SUCCESS if stream is closed successfuly, IO_FAILED otherwise.
 */
int io_close(void);

#endif /* ! IO_BACKEND_H */
