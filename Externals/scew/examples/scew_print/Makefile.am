#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Sat Nov 23, 2002 13:52
#
# Copyright (C) 2002-2009 Aleix Conchillo Flaque
#

INCLUDES = -I$(top_srcdir)

noinst_PROGRAMS = scew_print

scew_print_SOURCES = scew_print.c

if SCEW_UNICODE_WCHAR_T
scew_print_LDADD = $(top_builddir)/scew/libsceww.la
else
scew_print_LDADD = $(top_builddir)/scew/libscew.la
endif
