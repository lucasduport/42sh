lib_LIBRARIES = libcommand.a

libcommand_a_SOURCES =  \
    builtins/echo.c \
    builtins/builtins.h

libcommand_a_CPPFLAGS = -I$(top_srcdir)/src

libcommand_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libcommand.a
