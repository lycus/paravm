SUBDIRS = tests

AM_CPPFLAGS = -I "$(srcdir)/include"

lib_LTLIBRARIES = libparavm.la

libparavm_la_SOURCES = \
	src/common.c \
	src/lexer.c \
	src/opcodes.c

libparavm_la_LIBADD = @DEP_LIBS@ @DEP_PKG_LIBS@
libparavm_la_CFLAGS = @DEP_PKG_CFLAGS@
libparavm_la_LDFLAGS = -export-symbols-regex '^paravm_'

libparavmincludedir = $(includedir)/paravm

libparavminclude_HEADERS = \
	include/common.h \
	include/error.h \
	include/lexer.h \
	include/opcodes.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = paravm.pc

bin_PROGRAMS = paravm

paravm_LDADD = @DEP_LIBS@ @DEP_PKG_LIBS@
paravm_CFLAGS = @DEP_PKG_CFLAGS@

paravm_SOURCES = \
	src/main.c \
	src/tools.c

paravm_LDADD = libparavm.la

dist_man_MANS = man/paravm.1
