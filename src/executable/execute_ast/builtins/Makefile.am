lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
    alias.c \
    bool.c \
    cd.c \
    dot.c \
	echo.c \
    exit.c \
    export.c \
    stop_loop.c \
    unset.c \
    builtins.h

libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltins_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuiltins.a
