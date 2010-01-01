#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Wed Nov 06, 2002 00:55
#
# Copyright (C) 2002-2009 Aleix Conchillo Flaque
#

includedir = $(prefix)/include/$(PACKAGE)

include_HEADERS = attribute.h bool.h element.h error.h export.h \
	list.h parser.h	printer.h scew.h str.h tree.h \
	reader.h reader_buffer.h reader_file.h \
	writer.h writer_buffer.h writer_file.h

noinst_HEADERS = xattribute.h xelement.h xerror.h xparser.h

SCEW_SOURCES = attribute.c error.c list.c parser.c printer.c \
	element.c element_attribute.c element_compare.c \
	element_copy.c element_search.c str.c tree.c \
	xattribute.c xerror.c xparser.c \
	reader.c reader_buffer.c reader_file.c \
	writer.c writer_buffer.c writer_file.c

if SCEW_UNICODE_WCHAR_T

lib_LTLIBRARIES = libsceww.la
libsceww_la_SOURCES = $(SCEW_SOURCES)
libsceww_la_LDFLAGS = -version-info 1:2:0

else

lib_LTLIBRARIES = libscew.la
libscew_la_SOURCES = $(SCEW_SOURCES)
libscew_la_LDFLAGS = -version-info 1:2:0

endif
