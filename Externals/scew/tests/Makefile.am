#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Thu Aug 02, 2007 14:20
#
# Copyright (C) 2007-2009 Aleix Conchillo Flaque
#

SUBDIRS = data

INCLUDES = -I$(top_srcdir) -DSCEW_TESTSDIR=\""$(top_srcdir)/tests/data"\"

if CHECK_TESTS

CHECK_SCEW_CFLAGS = -std=gnu99

if SCEW_UNICODE_WCHAR_T

CHECK_SCEW_LIB = $(top_builddir)/scew/libsceww.la

else

CHECK_SCEW_LIB = $(top_builddir)/scew/libscew.la

endif

COMMON = main.c test.h

TESTS = check_attribute check_element check_list check_tree \
	check_reader_buffer check_reader_file \
	check_writer_buffer check_writer_file \
	check_parser check_printer

check_PROGRAMS = check_attribute check_element check_list check_tree \
	check_reader_buffer check_reader_file \
	check_writer_buffer check_writer_file \
	check_parser check_printer

# Attributes
check_attribute_SOURCES = $(COMMON) check_attribute.c \
	$(top_builddir)/scew/attribute.h $(top_builddir)/scew/element.h
check_attribute_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_attribute_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# Elements
check_element_SOURCES = $(COMMON) check_element.c \
	$(top_builddir)/scew/attribute.h $(top_builddir)/scew/element.h \
	$(top_builddir)/scew/list.h
check_element_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_element_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# Lists
check_list_SOURCES = $(COMMON) check_list.c $(top_builddir)/scew/list.h
check_list_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_list_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# Trees
check_tree_SOURCES = $(COMMON) check_tree.c \
	$(top_builddir)/scew/element.h $(top_builddir)/scew/tree.h
check_tree_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_tree_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# Buffer reader
check_reader_buffer_SOURCES = $(COMMON) check_reader_buffer.c \
	$(top_builddir)/scew/reader.h $(top_builddir)/scew/reader_buffer.h
check_reader_buffer_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_reader_buffer_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# File reader
check_reader_file_SOURCES = $(COMMON) check_reader_file.c \
	$(top_builddir)/scew/reader.h $(top_builddir)/scew/reader_file.h
check_reader_file_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_reader_file_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# Buffer writer
check_writer_buffer_SOURCES = $(COMMON) check_writer_buffer.c \
	$(top_builddir)/scew/writer.h $(top_builddir)/scew/writer_buffer.h
check_writer_buffer_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_writer_buffer_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# File writer
check_writer_file_SOURCES = $(COMMON) check_writer_file.c \
	$(top_builddir)/scew/writer.h $(top_builddir)/scew/writer_file.h
check_writer_file_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_writer_file_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# Printer
check_printer_SOURCES = $(COMMON) check_printer.c \
	$(top_builddir)/scew/writer.h $(top_builddir)/scew/writer_buffer.h \
	$(top_builddir)/scew/printer.h
check_printer_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_printer_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

# Parser
check_parser_SOURCES = $(COMMON) check_parser.c \
	$(top_builddir)/scew/parser.h $(top_builddir)/scew/reader.h \
	$(top_builddir)/scew/reader_file.h \
	$(top_builddir)/scew/reader_buffer.h \
	$(top_builddir)/scew/writer.h \
	$(top_builddir)/scew/writer_buffer.h
check_parser_CFLAGS = @CHECK_CFLAGS@ $(CHECK_SCEW_CFLAGS)
check_parser_LDADD = @CHECK_LIBS@ $(CHECK_SCEW_LIB)

else

check:
	@echo "WARNING: Check unit testing framework not available"

endif
