#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Sun Jul 06, 2003 04:51
#
# Copyright (C) 2002-2009 Aleix Conchillo Flaque
#

DOXYGEN_LOG = doxygen.log
API_DIR = api

if DOC

doc:
	mkdir -p $(API_DIR);
	@-$(DOXYGEN);

else

doc:
	@echo "WARNING: Documentation generation not enabled or available."

endif

clean:
	rm -rf $(API_DIR) $(DOXYGEN_LOG);
