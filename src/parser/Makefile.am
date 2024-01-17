lib_LIBRARIES = libparser.a

libparser_a_SOURCES =  \
  parser.c \
  parser_command.c \
  parser_if.c \
  parser_list.c \
  parser_redir.c \
  parser_loop.c \
  parser.h

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_LIBADD = $(top_builddir)/src/lexer/liblexer.a

noinst_LIBRARIES = libparser.a
