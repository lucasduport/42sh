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

# CFLAGS for compilation
CFLAGS="-std=c99 -Werror -Wall -Wextra -Wvla -pedantic"

# Define custom options for different build environments
AC_ARG_ENABLE([gdb],
  AS_HELP_STRING([--enable-gdb], [Build with debugging information for GDB]),
  [enable_gdb=yes],
  [enable_gdb=no])

AC_ARG_ENABLE([fsanitize],
  AS_HELP_STRING([--enable-fsanitize], [Build with address sanitizer]),
  [enable_fsanitize=yes],
  [enable_fsanitize=no])

if test "x$enable_gdb" = xyes; then
  CFLAGS="$CFLAGS -g3"
fi

if test "x$enable_fsanitize" = xyes; then
  CFLAGS="$CFLAGS -fsanitize=address -g3"
fi

# List Makefile in subdirectories
AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/lexer/Makefile
                    src/logger/Makefile
                    src/io_backend/Makefile
                    src/parser/Makefile
                    src/utils/Makefile
                    src/executable/command/Makefile
                    src/executable/execute_ast/Makefile
                    tests/Makefile
                ])
AC_OUTPUT
