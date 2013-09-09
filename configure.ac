AC_INIT([paravm], [0.0.0], [https://github.com/lycus/paravm/issues], [paravm], [http://paravm.org])
AC_CONFIG_SRCDIR([README])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST()

# Get rid of the default -g -O2 added by AC_PROG_CC.
CFLAGS="$CFLAGS"
AC_PROG_CC([clang cc])
AC_CONFIG_HEADERS([config.h prelude.h])

AC_MSG_CHECKING([whether the C compiler is Clang 3.3+])
AC_TRY_COMPILE(
    [],
    [#ifndef __clang__
     #    Unsupported C compiler.
     #endif

     #if __clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ < 3)
     #    Unsupported Clang version.
     #endif],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_FAILURE([the C compiler is not Clang 3.3+])])

AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])

LT_INIT()

AC_DEFUN([ADD_CFLAGS], [CFLAGS="$CFLAGS $1"])
AC_DEFUN([ADD_CPPFLAGS], [CPPFLAGS="$CPPFLAGS $1"])
AC_DEFUN([ADD_LDFLAGS], [LDFLAGS="$LDFLAGS $1"])

AC_DEFUN([CHECK_LIB],
    [AC_MSG_CHECKING([whether lib$2 is available])
     orig_ldflags="$LDFLAGS"
     ADD_LDFLAGS([-l$2])
     AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
         [$1_LIBS="-l$2"
          AC_SUBST([$1_LIBS])
          AC_MSG_RESULT([yes])],
         [$1_LIBS=""
          AC_SUBST([$1_LIBS])
          $3])
     LDFLAGS="$orig_ldflags"])

CHECK_LIB([DL], [dl], [AC_MSG_FAILURE([libdl is not available])])
CHECK_LIB([M], [m], [AC_MSG_FAILURE([libm is not available])])
CHECK_LIB([PTHREAD], [pthread], [AC_MSG_FAILURE([libpthread is not available])])
CHECK_LIB([E_HOST], [e_host])

PKG_CHECK_MODULES([GLIB], [glib-2.0])

AC_ARG_ENABLE(
    [debug],
    [AS_HELP_STRING([--enable-debug], [enable debug build [default=no]])],
    [],
    [enable_debug=no])

AS_IF(
    [test "x$enable_debug" != "xno"],
    [ADD_CFLAGS([-ggdb -O0 -fstack-protector-all])
     ADD_CPPFLAGS([-DPARAVM_DEBUG=1 -D_FORTIFY_SOURCE=2])],
    [ADD_CFLAGS([-O3])])

AC_ARG_ENABLE(
    [sanitize],
    [AS_HELP_STRING([--enable-sanitize=ARG], [enable a sanitizer [default=no]])],
    [AS_IF([test "x$enable_sanitize" = "xyes"],
           [AC_MSG_ERROR(you must give a sanitizer name to --enable-sanitize)],
           [])],
    [enable_sanitize=no])

AS_IF(
    [test "x$enable_sanitize" != "xno"],
    [ADD_CFLAGS([-fsanitize=$enable_sanitize])],
    [])

ADD_CFLAGS([-std=gnu11 -pthread -fms-extensions -fborland-extensions -fblocks])
ADD_CFLAGS([-fwrapv -fno-omit-frame-pointer -fexceptions -fno-strict-aliasing -funsigned-char])

# Ensure that only symbols we mark are exported.
ADD_CFLAGS([-fvisibility=hidden])

ADD_CFLAGS([-Wall -Wextra -Werror])
ADD_CFLAGS([-Winit-self -Wswitch-default -Wswitch-enum -Wundef -Wshadow -Wpointer-arith])
ADD_CFLAGS([-Wcast-qual -Wwrite-strings -Wconversion -Wstrict-prototypes -Wold-style-definition])
ADD_CFLAGS([-Wmissing-declarations -Wmissing-format-attribute -Wpadded -Wredundant-decls -Winline])

# Set feature macros as per feature_test_macros(7).
ADD_CPPFLAGS([-D_GNU_SOURCE=1 -D_FILE_OFFSET_BITS=64 -D_REENTRANT=1])

AC_CONFIG_FILES([Makefile paravm/Makefile paravm/paravm.pc:paravm/paravm.pc.in])

# We need to add this after the prelude has been generated.
ADD_CPPFLAGS([-include "$srcdir/prelude.h"])

AC_OUTPUT()
