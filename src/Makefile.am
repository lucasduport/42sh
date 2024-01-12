# define the sub directories
# FIXME : stop generating every binaries
SUBDIRS = \
		  logger \
		  io_backend \
		  utils \
		  lexer \
		  parser \
		  executable/execute_ast \
		  executable/command

bin_PROGRAMS = 42sh 42sh_gdb 42sh_fsanitize

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDFLAGS = -Wl,--start-group

42sh_LDADD =  \
	logger/liblogger.a \
	io_backend/libio_backend.a \
	lexer/liblexer.a \
	parser/libparser.a \
	utils/libutils.a \
	executable/command/libcommand.a \
	executable/execute_ast/libexecute.a

42sh_gdb_SOURCES = 42sh.c

42sh_gdb_CPPFLAGS = -I%D%

42sh_gdb_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g3

42sh_gdb_LDFLAGS = -Wl,--start-group

42sh_gdb_LDADD =  \
	logger/liblogger.a \
	io_backend/libio_backend.a \
	lexer/liblexer.a \
	parser/libparser.a \
	utils/libutils.a \
	executable/command/libcommand.a \
	executable/execute_ast/libexecute.a

42sh_fsanitize_SOURCES = 42sh.c

42sh_fsanitize_CPPFLAGS = -I%D%

42sh_fsanitize_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -g3 -fsanitize=address

42sh_fsanitize_LDFLAGS = -fsanitize=address -Wl,--start-group

42sh_fsanitize_LDADD =  \
	logger/liblogger.a \
	io_backend/libio_backend.a \
	lexer/liblexer.a \
	parser/libparser.a \
	utils/libutils.a \
	executable/command/libcommand.a \
	executable/execute_ast/libexecute.a

all: 42sh

gdb: 42sh_gdb

fsanitize: 42sh_fsanitize

clean-local:
	rm -f 42sh 42sh_gdb 42sh_fsanitize
