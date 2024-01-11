# define the sub directories
SUBDIRS = \
        logger \
        io_backend \
        utils \
        lexer \
        parser

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDFLAGS = -Wl,--start-group

42sh_LDADD =  \
       logger/liblogger.a \
       io_backend/libio_backend.a \
       lexer/liblexer.a \
       parser/libparser.a \
       utils/libutils.a

