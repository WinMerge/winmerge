@echo off

call configuration.bat

set docbook_inputfile=..\WinMerge_help.xml
set docbook_use_stylesheet=build_html.xsl
set docbook_outputdir=html

if not exist "%docbook_outputdir%" mkdir "%docbook_outputdir%"

echo Copy images...
if not exist "%docbook_outputdir%\images" mkdir "%docbook_outputdir%\images"
copy "..\images\*.png" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "..\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "..\css\all.css" "%docbook_outputdir%\css\all.css"
copy "..\css\print.css" "%docbook_outputdir%\css\print.css"

echo Create HTML files...
%docbook_java_exe% %docbook_java_parameters% -cp %docbook_saxon_jar%;%docbook_saxon_xsl% com.icl.saxon.StyleSheet %docbook_inputfile% %docbook_use_stylesheet% base.dir=%docbook_outputdir%\

echo Finished!

@echo on