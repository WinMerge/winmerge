#
#	Initial configuration for Win32
#

BIN				= ..\..\..\Build\pcre\bin
SOURCES			= ..\pcre-6.7

H_config		= $(BIN)\config.h
I_config		= $(SOURCES)\config.h.in

C_dftables		= $(SOURCES)\dftables.c
X_dftables		= $(BIN)\dftables.exe

C_chartables	= $(BIN)\pcre_chartables.c

TARGETS			= $(H_config) $(C_chartables)

all : $(TARGETS)

clean : ; -del $(TARGETS)

rebuild : clean all

#
#	Generate config.h for Win32
#

$(H_config) : $(I_config) configure.mak
	if not exist $(BIN) mkdir $(BIN)
	copy $(I_config) + << $@

/* Automatically added for Win32 compilation */
#define SUPPORT_UCP
#define SUPPORT_UTF8

<<
	edlin $@ < <<
1S#define HAVE_MEMMOVE
.
#define HAVE_MEMMOVE 1
1S#define HAVE_STRERROR
.
#define HAVE_STRERROR 1
E
<<

#
#	Generate pcre_chartables.c by a compile and run of dftables
#

$(X_dftables) : $(C_dftables) $(H_config)
	cl /nologo /I$(BIN) /Fe$@ $(C_dftables)

$(C_chartables) : $(X_dftables)
	$(X_dftables) $@
