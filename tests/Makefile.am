check_PROGRAMS = testsIo

testsIo_SOURCES = tests_io.c

testsIo_LDADD = -lcriterion \
			$(top_srcdir)/src/io_backend/libio_backend.a \
			$(top_srcdir)/src/logger/liblogger.a

check-io: testsIo
	./testsIo

check-local: testsIo
	./$(check_PROGRAMS)
	rm -f *.txt
	python3 launch_tests.py --verbose tests_files/builtin_tests.yaml

clean-local:
	rm -f testsIo
