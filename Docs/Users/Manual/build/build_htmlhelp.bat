@echo off

call configuration.bat

set docbook_inputfile=..\WinMerge_help.xml
set docbook_use_stylesheet=build_htmlhelp.xsl
set docbook_outputdir=htmlhelp

if not exist "%docbook_outputdir%" mkdir "%docbook_outputdir%"

echo Copy images...
if not exist "%docbook_outputdir%\images" mkdir "%docbook_outputdir%\images"
copy "..\images\*.gif" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "..\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "..\css\help.css" "%docbook_outputdir%\css\help.css"

echo Create HTML files...
%docbook_java_exe% %docbook_java_parameters% -cp %docbook_saxon_jar%;%docbook_saxon_xsl% com.icl.saxon.StyleSheet %docbook_inputfile% %docbook_use_stylesheet% base.dir=%docbook_outputdir%\
if exist "htmlhelp.hhp" goto compile

:compile
echo Compile HTML Help...
%docbook_hhc_exe% "htmlhelp.hhp"

echo Cleaning...
deltree /Y "%docbook_outputdir%\images"
deltree /Y "%docbook_outputdir%\screenshots"
deltree /Y "%docbook_outputdir%\css"
del "%docbook_outputdir%\*.html"
del "htmlhelp.hhp"
del "toc.hhc"
move "htmlhelp.chm" "%docbook_outputdir%\WinMerge.chm"

echo Finished!
goto end

:error
echo ERORR!!!
goto end

:end
@echo on