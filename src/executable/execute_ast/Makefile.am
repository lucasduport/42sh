lib_LIBRARIES = libexecute.a

libexecute_a_SOURCES =  \
	execute.c \
    execute_pipe.c \
    execute_redir.c \
    execute.h

libexecute_a_CPPFLAGS = -I$(top_srcdir)/src

libexecute_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libexecute.a
