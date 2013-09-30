SUBDIRS = tests

lib_LTLIBRARIES = libparavm.la

libparavm_la_SOURCES = \
	include/internal/atomic.h \
	include/internal/common.h \
	src/assemble.c \
	src/atom.c \
	src/common.c \
	src/context.c \
	src/error.c \
	src/ir.c \
	src/lexer.c \
	src/opcodes.c

libparavm_la_LIBADD = @DEP_LIBS@ @DEP_PKG_LIBS@
libparavm_la_CFLAGS = @DEP_PKG_CFLAGS@
libparavm_la_LDFLAGS = -export-symbols-regex "^paravm_"

libparavmincludedir = $(includedir)/paravm

libparavminclude_HEADERS = \
	include/assemble.h \
	include/atom.h \
	include/common.h \
	include/context.h \
	include/error.h \
	include/ir.h \
	include/lexer.h \
	include/opcodes.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pc/paravm.pc

bin_PROGRAMS = paravm

paravm_SOURCES = \
	include/internal/options.h \
	include/internal/tools.h \
	src/main.c \
	src/tools.c

paravm_LDADD = @DEP_LIBS@ @DEP_PKG_LIBS@ libparavm.la
paravm_CFLAGS = @DEP_PKG_CFLAGS@
paravm_LDFLAGS = -export-dynamic

man_MANS = man/paravm.1

EXTRA_DIST = \
	vim/ftdetect/pva.vim \
	vim/syntax/pva.vim

analyze_srcs = $(filter %.c, $(libparavm_la_SOURCES) $(paravm_SOURCES))
analyze_plists = $(analyze_srcs:%.c=%.plist)

MOSTLYCLEANFILES = $(analyze_plists)

all-local: $(analyze_plists)

$(analyze_plists): %.plist: %.c
	@echo "  CCSA  " $@
	@$(COMPILE) @DEP_PKG_CFLAGS@ --analyze $< -o $@
