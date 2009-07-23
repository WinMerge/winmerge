REM Run Astyle to format source code
REM Get Astyle from:
REM http://astyle.sourceforge.net/
REM $Id$

REM Src is Ansi-style - tab-indent
astyle --style=ansi --indent=tab --pad-oper --unpad-paren Src/*.cpp
astyle --style=ansi --indent=tab --pad-oper --unpad-paren Src/*.h

astyle --style=ansi --indent=tab --pad-oper --unpad-paren Src/Common/*.cpp
astyle --style=ansi --indent=tab --pad-oper --unpad-paren Src/Common/*.h

astyle --style=ansi --indent=tab --pad-oper --unpad-paren Src/CompareEngines/*.cpp
astyle --style=ansi --indent=tab --pad-oper --unpad-paren Src/CompareEngines/*.h

REM Editor code is GNU style - 2 space indent
astyle --style=gnu --pad-oper --unpad-paren Src/editlib/*.cpp
astyle --style=gnu --pad-oper --unpad-paren Src/editlib/*.h