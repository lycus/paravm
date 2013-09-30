SUBDIRS = tests

lib_LTLIBRARIES = libparavm.la

libparavm_la_SOURCES = \
	include/internal/atomic.h \
	include/internal/common.h \
	src/assemble.c \
	src/atom.c \
	src/common.c \
	src/context.c \
	src/disassemble.c \
	src/error.c \
	src/io.c \
	src/ir.c \
	src/lex.c \
	src/opcode.c \
	src/verify.c

libparavm_la_LIBADD = @DEP_LIBS@ @DEP_PKG_LIBS@
libparavm_la_CFLAGS = @DEP_PKG_CFLAGS@
libparavm_la_LDFLAGS = -export-symbols-regex "^paravm_"

libparavmincludedir = $(includedir)/paravm

libparavminclude_HEADERS = \
	include/assemble.h \
	include/atom.h \
	include/common.h \
	include/context.h \
	include/disassemble.h \
	include/error.h \
	include/io.h \
	include/ir.h \
	include/lex.h \
	include/opcode.h \
	include/verify.h

libparavminclude_DATA = map/module.map

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pc/paravm.pc

bin_PROGRAMS = paravm

paravm_SOURCES = \
	include/internal/option.h \
	include/internal/tool.h \
	src/main.c \
	src/tool.c

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
