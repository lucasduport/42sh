lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
    bool.c \
    dot.c \
	echo.c \
    exit.c \
    export.c \
    unset.c \
    stop_loop.c \
    builtins.h

libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltins_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuiltins.a
