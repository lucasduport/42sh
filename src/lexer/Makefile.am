lib_LIBRARIES = liblexer.a

liblexer_a_SOURCES =  \
  lexer_rules.c \
  lexer_tools.c \
  lexer.c \
  lexer.h \
  token.c \
  token.h

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = liblexer.a
