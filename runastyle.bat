REM Run Astyle to format source code
REM Get Astyle from:
REM http://astyle.sourceforge.net/
REM $Id$

REM Src is Ansi-style - tab-indent
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Src/*.cpp
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Src/*.h

astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Src/Common/*.cpp
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Src/Common/*.h

astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Src/CompareEngines/*.cpp
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Src/CompareEngines/*.h

astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks ShellExtension/*.cpp
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks ShellExtension/*.h

REM Editor code is GNU style - 2 space indent
astyle --style=gnu --pad-oper --unpad-paren Src/editlib/*.cpp
astyle --style=gnu --pad-oper --unpad-paren Src/editlib/*.h
