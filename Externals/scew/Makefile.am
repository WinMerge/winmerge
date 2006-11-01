#
# $Id: Makefile.am,v 1.11 2004/01/29 22:42:09 aleix Exp $
#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Mon Nov 04, 2002 20:29
#
# Copyright (C) 2002, 2003, 2004 Aleix Conchillo Flaque
#

SUBDIRS = doc etc win32 scew examples

EXTRA_DIST = INSTALL.WIN32

.PHONY: doc

if DOC

doc:
	(cd doc && $(MAKE) doc)

else

doc:
	@echo "Doxygen documentation generation not enabled";

endif
