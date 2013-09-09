AM_CPPFLAGS = -I "$(srcdir)/include"

lib_LTLIBRARIES = libparavm.la

libparavm_la_SOURCES = \
	src/common.c

libparavm_la_LIBADD = -ldl -lm @GLIB_LIBS@ -le_host
libparavm_la_CFLAGS = @GLIB_CFLAGS@
libparavm_la_LDFLAGS = -pthread -export-symbols-regex '^paravm_'

libparavmincludedir = $(includedir)/paravm

libparavminclude_HEADERS = \
	include/common.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = paravm.pc

bin_PROGRAMS = paravm

paravm_SOURCES = \
	src/main.c \
	src/tools.c

paravm_LDADD = libparavm.la

dist_man_MANS = man/paravm.1
