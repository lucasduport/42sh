#ifndef IO_BACKEND_H
#define IO_BACKEND_H

#define IO_SUCCESS 0
#define IO_FAILED 1


int io_abstraction(int argc, char *argv[]);
char io_getchar(void);
int io_close(void);

#endif /* ! IO_BACKEND_H */
