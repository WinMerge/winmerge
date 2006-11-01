#
# Makefile for AmigaOS
#

.PHONY: help all staticlib sharedlib check clean install

vpath %.c ../lib ../examples ../xmlwf ../tests
vpath %.h ../lib ../tests


#
# Set these for SDK installation.
#
tooldir = SDK:Local/C
staticlibdir = SDK:Local/clib2/lib
staticincludedir = SDK:Local/clib2/include
sharedlibdir = LIBS:
sharedincludedir = SDK:Local/common/include


ifeq ($(MAKECMDGOALS), staticlib)
 DESTDIR = lib
else
 DESTDIR = libs
endif

STATICLIB = $(DESTDIR)/libexpat.a
SHAREDLIB = $(DESTDIR)/expat.library

XMLWF = $(DESTDIR)/xmlwf
ELEMENTS = $(DESTDIR)/elements
OUTLINE = $(DESTDIR)/outline
RUNTESTS = $(DESTDIR)/runtests
BENCHMARK = $(DESTDIR)/benchmark


help:
	@echo "Requires:"
	@echo " AmigaOS 4.0"
	@echo " SDK 51.15"
	@echo " clib2 1.198"
	@echo ""
	@echo "Targets:"
	@echo " all - make expat libraries, xmlwf, examples and run test suite"
	@echo " install - install expat libraries and tools into SDK"

all:
	-makedir lib libs
	$(MAKE) staticlib
	$(MAKE) sharedlib

staticlib: $(STATICLIB) $(XMLWF) $(ELEMENTS) $(OUTLINE) $(RUNTESTS) $(BENCHMARK) check
	@echo done

sharedlib: $(SHAREDLIB) $(XMLWF) $(ELEMENTS) $(OUTLINE) $(RUNTESTS) $(BENCHMARK) check
	@echo done

check:
	$(RUNTESTS)

clean:
	-delete lib/#?.o quiet
	-delete libs/#?.o quiet

install:
	copy lib/xmlwf $(tooldir) quiet
	copy /lib/expat.h /lib/expat_external.h $(staticincludedir) quiet
	copy lib/libexpat.a $(staticlibdir) quiet
	copy libs/expat.library $(sharedlibdir) quiet
	copy include $(sharedincludedir) quiet all


CC = gcc
LIBTOOL = ar
STRIP = strip

LDFLAGS =
LIBS =
CFLAGS = -DNDEBUG -O2
LTFLAGS = -crs
STRIPFLAGS = -R.comment
LAUNCH =

ifeq ($(MAKECMDGOALS), staticlib)
 LIBS += $(STATICLIB)
 INCLUDES = -I../lib
endif

ifeq ($(MAKECMDGOALS), sharedlib)
 CFLAGS += -DAMIGA_SHARED_LIB -D__USE_INLINE__
 INCLUDES = -I. -Iinclude -Iinclude/libraries -I../lib
 LAUNCH = $(DESTDIR)/launch.o
endif


COMPILE = $(CC) $(CFLAGS) $(INCLUDES) -c

LINK = $(CC) $(LDFLAGS) -o $@


$(STATICLIB): $(DESTDIR)/xmlparse.o $(DESTDIR)/xmltok.o $(DESTDIR)/xmlrole.o
	$(LIBTOOL) $(LTFLAGS) $@ $^

$(DESTDIR)/xmlparse.o: xmlparse.c expat.h xmlrole.h xmltok.h \
	expat_external.h internal.h amigaconfig.h
	$(COMPILE) $< -o $@

$(DESTDIR)/xmlrole.o: xmlrole.c ascii.h xmlrole.h expat_external.h \
	internal.h amigaconfig.h
	$(COMPILE) $< -o $@

$(DESTDIR)/xmltok.o: xmltok.c xmltok_impl.c xmltok_ns.c ascii.h asciitab.h \
	iasciitab.h latin1tab.h nametab.h utf8tab.h xmltok.h xmltok_impl.h \
	expat_external.h internal.h amigaconfig.h
	$(COMPILE) $< -o $@


$(SHAREDLIB): $(DESTDIR)/expat_lib.o $(DESTDIR)/expat_vectors.o $(DESTDIR)/stdlib.o lib/libexpat.a
	$(LINK) -nostartfiles -nostdlib -o $@ $^

$(DESTDIR)/expat_lib.o: expat_lib.c
	$(COMPILE) $< -o $@

$(DESTDIR)/expat_vectors.o: expat_vectors.c
	$(COMPILE) $< -o $@

$(DESTDIR)/stdlib.o: stdlib.c
	$(COMPILE) $< -o $@

$(LAUNCH): launch.c
	$(COMPILE) $< -o $@


$(XMLWF): $(DESTDIR)/xmlwf.o $(DESTDIR)/xmlfile.o $(DESTDIR)/codepage.o $(DESTDIR)/readfilemap.o $(LAUNCH)
	$(LINK) $^ $(LIBS)
	$(STRIP) $(STRIPFLAGS) $@

$(DESTDIR)/xmlwf.o: xmlwf.c
	$(COMPILE) $< -o $@

$(DESTDIR)/xmlfile.o: xmlfile.c
	$(COMPILE) $< -o $@

$(DESTDIR)/codepage.o: codepage.c
	$(COMPILE) $< -o $@

$(DESTDIR)/readfilemap.o: readfilemap.c
	$(COMPILE) $< -o $@


$(ELEMENTS): $(DESTDIR)/elements.o $(LAUNCH)
	$(LINK) $^ $(LIBS)
	$(STRIP) $(STRIPFLAGS) $@

$(DESTDIR)/elements.o: elements.c
	$(COMPILE) $< -o $@


$(OUTLINE): $(DESTDIR)/outline.o $(LAUNCH)
	$(LINK) $^ $(LIBS)
	$(STRIP) $(STRIPFLAGS) $@

$(DESTDIR)/outline.o: outline.c
	$(COMPILE) $< -o $@


$(RUNTESTS): $(DESTDIR)/runtests.o $(DESTDIR)/chardata.o $(DESTDIR)/minicheck.o $(LAUNCH)
	$(LINK) $^ $(LIBS)

$(DESTDIR)/chardata.o: chardata.c chardata.h
	$(COMPILE) $< -o $@

$(DESTDIR)/minicheck.o: minicheck.c minicheck.h
	$(COMPILE) $< -o $@

$(DESTDIR)/runtests.o: runtests.c chardata.h
	$(COMPILE) $< -o $@


$(BENCHMARK): $(DESTDIR)/benchmark.o $(LAUNCH)
	$(LINK) $^ $(LIBS) -lm

$(DESTDIR)/benchmark.o: benchmark/benchmark.c
	$(COMPILE) $< -o $@
