bin_PROGRAMS = 42sh

# Define the sub directories
SUBDIRS = \
		  logger \
		  io_backend \
		  utils \
		  lexer \
		  parser \
		  executable/execute_ast \
		  executable/command \
		  executable/environment

# Common flags for all targets
AM_CPPFLAGS = -I%D%

42sh_LDFLAGS = -Wl,--start-group

42sh_LDADD =  \
	logger/liblogger.a \
	io_backend/libio_backend.a \
	lexer/liblexer.a \
	parser/libparser.a \
	utils/libutils.a \
	executable/command/libcommand.a \
	executable/execute_ast/libexecute.a

42sh_SOURCES = 42sh.c

clean-local:
	rm -f 42sh
