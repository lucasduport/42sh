check_PROGRAMS = criterion_lexer criterion_io

criterion_lexer_SOURCES = tests_lexer.c
criterion_lexer_CPP = -I$(top_srcdir)/src

criterion_lexer_LDADD = -lcriterion \
                $(top_builddir)/src/liblexer.a

criterion_io_SOURCES = tests_io.c
criterion_io_CPP = -I$(top_srcdir)/src

criterion_io_LDADD = -lcriterion \
                $(top_builddir)/src/libio_backend.a

check-lexer: criterion_lexer
    ./criterion_lexer

check-io: criterion_io
    ./criterion_io

check-all: criterion_io criterion_lexer
    ./criterion_io
    ./criterion_lexer
    ./launch_tests.sh
