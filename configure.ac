AC_PREREQ([2.63])
AC_INIT([horient], [0.0.1], [krbschmitt@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/horient.hpp]) 
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])

# Checks for programs.
AC_PROG_CXX

# Check for yaggo
AC_ARG_VAR([YAGGO], [Yaggo switch parser generator])
AS_IF([test "x$YAGGO" = "x"], [AC_PATH_PROG([YAGGO], [yaggo], [false])])

AC_OUTPUT([Makefile])
