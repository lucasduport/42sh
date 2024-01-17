bin_PROGRAMS = 42sh

# Define the sub directories
SUBDIRS = \
		  logger \
		  io_backend \
		  utils \
		  lexer \
		  parser \
		  expansion \
		  executable/execute_ast \
		  executable/command \
		  executable/environment

# Common flags for all targets
AM_CPPFLAGS = -I%D%

AM_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
	executable/environment/libenvironment.a \
	executable/execute_ast/libexecute.a \
	executable/command/libcommand.a \
	parser/libparser.a \
	lexer/liblexer.a \
	logger/liblogger.a \
	utils/libutils.a \
	io_backend/libio_backend.a \
	expansion/libexpansion.a

42sh_SOURCES = 42sh.c

clean-local:
	rm -f 42sh
