#
# $Id: Makefile.am,v 1.1 2004/01/28 00:43:21 aleix Exp $
#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Wed Nov 06, 2002 00:55
#
# Copyright (C) 2002, 2003, 2004 Aleix Conchillo Flaque
#

includedir = $(prefix)/include/$(PACKAGE)

include_HEADERS = scew.h attribute.h element.h error.h parser.h str.h \
                  tree.h types.h writer.h

noinst_HEADERS = xattribute.h xelement.h xerror.h xhandler.h xparser.h \
                 xprint.h xtree.h

lib_LIBRARIES = libscew.a

libscew_a_SOURCES = attribute.c element.c error.c parser.c str.c tree.c \
                    writer.c xattribute.c xerror.c xhandler.c xparser.c xprint.c
