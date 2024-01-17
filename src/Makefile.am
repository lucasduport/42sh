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
<<<<<<< HEAD
		  executable/redirection
=======
		  executable/environment
>>>>>>> bd4108c48bee018fe7c0b0888a7d360c183ef1c1

# Common flags for all targets
AM_CPPFLAGS = -I%D%

AM_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
	executable/environment/libenvironment.a \
	executable/execute_ast/libexecute.a \
	executable/command/libcommand.a \
<<<<<<< HEAD
	executable/execute_ast/libexecute.a \
	executable/redirection/libredir.a
=======
	parser/libparser.a \
	lexer/liblexer.a \
	logger/liblogger.a \
	utils/libutils.a \
	io_backend/libio_backend.a \
	expansion/libexpansion.a
>>>>>>> bd4108c48bee018fe7c0b0888a7d360c183ef1c1

42sh_SOURCES = 42sh.c

clean-local:
	rm -f 42sh
