check_PROGRAMS = testsIo testsLogger

testsIo_SOURCES = tests_io.c

testsIo_LDADD = -lcriterion \
			$(top_srcdir)/src/io_backend/libio_backend.a \
			$(top_srcdir)/src/logger/liblogger.a

testsLogger_SOURCES = tests_logger.c

testsLogger_LDADD = -lcriterion \
			$(top_srcdir)/src/logger/liblogger.a \
			$(top_srcdir)/src/utils/libutils.a
			

check-local: $(check_PROGRAMS)
	./testsIo
	./testsLogger
	rm -f *.txt
	python3 launch_tests.py --verbose tests_files

clean-local:
	rm -f $(check_PROGRAMS)
