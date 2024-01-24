lib_LIBRARIES = libenvironment.a

libenvironment_a_SOURCES =  \
     variable.c \
	environment.c \
    special_var.c \
    environment.h

libenvironment_a_CPPFLAGS = -I$(top_srcdir)/src

libenvironment_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libenvironment.a
