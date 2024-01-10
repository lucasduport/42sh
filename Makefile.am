# define the sub directories
SUBDIRS = \
        src/logger

bin_PROGRAMS = 42sh

42sh_SOURCES = src/42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD = \
       src/logger/liblogger.a

