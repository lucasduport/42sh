#include "logger.h"

// Global variable to store the logger object
static struct my_logs *logger = NULL;

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
    if (logger == NULL)
        return;
    if (fd != stdout)
        fclose(fd);
}

int enable_log_type(enum log_type type)
{
    if (logger == NULL)
        return false;
    if (type < 0 || type >= NB_TYPES)
        return false;
    logger->types_to_log[type] = true;
    return true;
}

int disable_log_type(enum log_type type)
{
    if (logger == NULL)
        return false;
    if (type < 0 || type >= NB_TYPES)
        return false;
    logger->types_to_log[type] = false;
    return true;
}

int enable_all_logs(void)
{
    if (logger == NULL)
        return false;
    for (int i = 0; i < NB_TYPES; i++)
        logger->types_to_log[i] = true;
    return true;
}

int disable_all_logs(void)
{
    if (logger == NULL)
        return false;
    for (int i = 0; i < NB_TYPES; i++)
        logger->types_to_log[i] = false;
    return true;
}

int debug_printf(enum log_type type, const char *format, ...)
{
    FILE *fd = open_logger();
    if (fd == NULL || !logger->types_to_log[type])
        return false;
    va_list args;
    va_list args_copy;
    va_start(args, format);
    va_copy(args_copy, args);
    if (vfprintf(fd, format, args_copy) < 0)
        return false;
    va_end(args);
    va_end(args_copy);
    close_logger(fd);
    return true;
}

int destroy_logger(void)
{
    if (logger == NULL)
        return true;
    if (logger->filename)
        free(logger->filename);
    free(logger);
    return true;
}
