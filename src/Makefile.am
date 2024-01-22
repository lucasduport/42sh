bin_PROGRAMS = 42sh

# Define the sub directories
SUBDIRS = \
		  logger \
		  io_backend \
		  utils \
		  lexer \
		  parser \
		  expansion \
		  executable

# Common flags for all targets
AM_CPPFLAGS = -I%D%

AM_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD = \
	executable/execute_ast/libexecute.a \
	parser/libparser.a \
	lexer/liblexer.a \
	logger/liblogger.a \
	io_backend/libio_backend.a \
	executable/environment/libenvironment.a \
	executable/execute_ast/builtins/libbuiltins.a \
	expansion/libexpansion.a \
	utils/libutils.a

42sh_SOURCES = 42sh.c

clean-local:
	rm -f 42sh
