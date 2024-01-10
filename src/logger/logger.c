#include "logger.h"

// Global variable to store the logger object
static struct my_logs *logger = NULL;

/**
 * @brief Get the logger object
 *
 * @return the logger object
 */
static struct my_logs *get_logger(void)
{
    return logger;
}

int create_logger(char *filename)
{
    struct my_logs *l = calloc(1, sizeof(struct my_logs));
    if (l == NULL)
        return false;
    if (filename)
    {
        l->filename = strdup(filename);
        logger = l;
    }
    return true;
}

/**
 * @brief Opens the logger
 *
 * @return the logger file stream
 */
static FILE *open_logger(void)
{
    struct my_logs *logger = get_logger();
    if (logger == NULL)
        return NULL;
    FILE *fd = stdout;
    if (logger->filename)
    {
        if (strcmp(logger->filename, "stdout") == 0)
            return fd;
        fd = fopen(logger->filename, "a");
        if (fd == NULL)
            return NULL;
    }
    return fd;
}

/**
 * @brief Closes the logger
 *
 * @param fd the logger file stream
 */
static void close_logger(FILE *fd)
{
    struct my_logs *logger = get_logger();
    if (logger == NULL)
        return;
    if (fd != stdout)
        fclose(fd);
}

int debug_printf(const char *format, ...)
{
    FILE *fd = open_logger();
    if (fd == NULL)
        return false;
    va_list args;
    va_list args_copy;
    va_start(args, format);
    va_copy(args_copy, args);
    if (vfprintf(fd, format, args_copy) < 0)
        return false;
    if (fprintf(fd, "\n") < 0)
        return false;
    va_end(args);
    va_end(args_copy);
    close_logger(fd);
    return true;
}

int destroy_logger(void)
{
    struct my_logs *logger = get_logger();
    if (logger == NULL)
        return true;
    if (logger->filename)
        free(logger->filename);
    free(logger);
    return true;
}
