AC_INIT([paravm], [0.0.0], [https://github.com/lycus/paravm/issues], [paravm], [http://paravm.org])
AC_CANONICAL_HOST()

# Get rid of the default -g -O2 added by AC_PROG_CC.
CFLAGS="$CFLAGS"

AC_PROG_CC([clang cc])
AC_CONFIG_HEADERS([config.h prelude.h])

AC_MSG_CHECKING([whether the C compiler is Clang 3.3+])
AC_TRY_COMPILE(
    [],
    [#ifndef __clang__
     #    error "Unsupported C compiler."
     #endif

     #if __clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ < 3)
     #    error "Unsupported Clang version."
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
    [AC_MSG_CHECKING([whether lib$1 is available])

     orig_ldflags="$LDFLAGS"
     ADD_LDFLAGS([-l$1])

     AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
         [DEP_LIBS="$DEP_LIBS -l$1"
          have_lib$2=yes

          AC_MSG_RESULT([yes])],
         [have_lib$2=no

          $3])

     LDFLAGS="$orig_ldflags"])

AC_DEFUN([CHECK_PKG],
    [PKG_CHECK_MODULES([$1], [$2])

     DEP_PKG_NAMES="$DEP_PKG_NAMES $2"
     DEP_PKG_CFLAGS="$DEP_PKG_CFLAGS $$1_CFLAGS"
     DEP_PKG_LIBS="$DEP_PKG_LIBS $$1_LIBS"])

CHECK_LIB([dl], [dl], [AC_MSG_FAILURE([libdl is not available])])
CHECK_LIB([m], [m], [AC_MSG_FAILURE([libm is not available])])
CHECK_LIB([pthread], [pthread], [AC_MSG_FAILURE([libpthread is not available])])
CHECK_LIB([BlocksRuntime], [blocksruntime], [AC_MSG_FAILURE([libBlocksRuntime is not available])])
CHECK_LIB([gmp], [gmp], [AC_MSG_FAILURE([libgmp is not available])])
CHECK_LIB([e-hal], [e_hal], [AC_MSG_RESULT([no (building without the JIT compiler)])
                             AC_DEFINE([PARAVM_EMULATE], [1], [Define to 1 if libe-hal is not available.])])

AM_CONDITIONAL([PARAVM_EMULATE], [test "x$have_libe_hal" = "xno"])

CHECK_PKG([libglib], [glib-2.0])
CHECK_PKG([libffi], [libffi])
CHECK_PKG([libgc], [bdw-gc])
CHECK_PKG([libedit], [libedit])

AC_SUBST([DEP_LIBS])
AC_SUBST([DEP_PKG_NAMES])
AC_SUBST([DEP_PKG_CFLAGS])
AC_SUBST([DEP_PKG_LIBS])

AC_ARG_ENABLE(
    [debug],
    [AS_HELP_STRING([--enable-debug], [enable debug build [default=no]])],
    [],
    [enable_debug=no])

AS_IF(
    [test "x$enable_debug" != "xno"],
    [ADD_CFLAGS([-ggdb -O0 -fstack-protector-all])
     ADD_CPPFLAGS([-D_FORTIFY_SOURCE=2])
     AC_DEFINE([PARAVM_DEBUG], [1], [Define to 1 to build in debug mode.])],
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

ADD_CFLAGS([-std=gnu11 -pthread -fblocks])
ADD_CFLAGS([-fwrapv -fno-omit-frame-pointer -fexceptions -fno-strict-aliasing])

# Ensure that only symbols we mark are exported.
ADD_CFLAGS([-fvisibility=hidden])

ADD_CFLAGS([-Wall -Wextra -Werror])
ADD_CFLAGS([-Winit-self -Wswitch-default -Wundef -Wshadow -Wpointer-arith -Wcast-qual])
ADD_CFLAGS([-Wwrite-strings -Wconversion -Wstrict-prototypes -Wold-style-definition])
ADD_CFLAGS([-Wmissing-declarations -Wmissing-format-attribute -Wredundant-decls -Winline])

# Set feature macros as per feature_test_macros(7).
ADD_CPPFLAGS([-D_GNU_SOURCE=1 -D_FILE_OFFSET_BITS=64 -D_REENTRANT=1])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([paravm/Makefile])
AC_CONFIG_FILES([paravm/tests/Makefile])

AC_CONFIG_FILES([paravm/map/module.map])
AC_CONFIG_FILES([paravm/pc/paravm.pc])
AC_CONFIG_FILES([paravm/man/paravm.1])

# We need to add this after the prelude has been generated.
ADD_CPPFLAGS([-include $srcdir/prelude.h -I$srcdir/include])

AC_OUTPUT()
