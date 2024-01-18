# Init the 42sh project
AC_INIT([42sh], [0.1], [lucas.duport@epita.fr])

# Setup automake
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# Pretty display of makefile rules
AM_SILENT_RULES([no])

# Enable ar for Makefile
AM_PROG_AR

# Check if a ranlib is available
AC_PROG_RANLIB

# Check if a compiler is available for c
AC_PROG_CC

# Define custom options for different build environments
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Build with debugging information for GDB and fsanitize]),
  [enable_debug=yes],
  [enable_debug=no])

AC_ARG_ENABLE([coverage],
  AS_HELP_STRING([--enable-coverage], [Build to test coverage]),
  [enable_cov=yes],
  [enable_cov=no])

if test "x$enable_debug" = xyes; then
  CFLAGS="-fsanitize=address -g3"
fi

if test "x$enable_cov" = xyes; then
  CFLAGS="-fPIC -fprofile-arcs -ftest-coverage"
  LFAGS="-lgcov"
fi

# List Makefile in subdirectories
AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/lexer/Makefile
                    src/logger/Makefile
                    src/io_backend/Makefile
                    src/parser/Makefile
                    src/expansion/Makefile
                    src/utils/Makefile
                    src/executable/execute_ast/Makefile
                    src/executable/environment/Makefile
                    tests/Makefile
                ])
AC_OUTPUT
