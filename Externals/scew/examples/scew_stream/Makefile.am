#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Sun May 23, 2004 20:57
#
# Copyright (C) 2002-2009 Aleix Conchillo Flaque
#

INCLUDES = -I$(top_srcdir)

noinst_PROGRAMS = scew_stream

scew_stream_SOURCES = scew_stream.c

if SCEW_UNICODE_WCHAR_T
scew_stream_LDADD = $(top_builddir)/scew/libsceww.la
else
scew_stream_LDADD = $(top_builddir)/scew/libscew.la
endif
