lib_LIBRARIES = libutils.a

libutils_a_SOURCES =  \
    ast/ast.c \
    ast/ast_print.c \
    ast/ast.h \
    list/list.c \
    list/list.h \
    string/string.c \
    string/string.h \
    stack/stack.c \
    stack/stack.h

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libutils.a
