!include <win32.mak>

PROJ=StackWalker
ALL : $(OUTDIR) $(OUTDIR)\$(PROJ).exe

$(OUTDIR) :
	if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32_OBJS= \
	$(OUTDIR)\StackWalker.obj \
	$(OUTDIR)\main.obj


# Generic rule for building ALL CPP files and placing their OBJ's in the OUTDIR
$(OUTDIR)\StackWalker.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) /EHsc /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" /Fp$(OUTDIR)\ StackWalker.cpp

$(OUTDIR)\main.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) /EHsc /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" /Fp$(OUTDIR)\ main.cpp

$(OUTDIR)\$(PROJ).exe:  $(LINK32_OBJS)
    $(link) $(ldebug) $(conlflags) /PDB:$(OUTDIR)\$(PROJ).pdb -out:$(OUTDIR)\$(PROJ).exe $(LINK32_OBJS) $(conlibs) ole32.lib

CLEAN :
	$(CLEANUP)

