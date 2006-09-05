@echo off

call configuration.bat

set docbook_inputfile=..\WinMerge_help.xml
set docbook_use_stylesheet=build_htmlhelp.xsl
set docbook_outputdir=htmlhelp
set docbook_outputdir_final=%docbook_build_path%\%docbook_outputdir%

if not exist "%docbook_outputdir%" mkdir "%docbook_outputdir%"
if not exist "%docbook_outputdir_final%" mkdir "%docbook_outputdir_final%"

echo Copy images...
if not exist "%docbook_outputdir%\images" mkdir "%docbook_outputdir%\images"
copy "..\images\*.gif" "%docbook_outputdir%\images\."
copy "..\images\*.png" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "..\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy art...
if not exist "%docbook_outputdir%\art" mkdir "%docbook_outputdir%\art"
copy "..\art\*.*" "%docbook_outputdir%\art\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "..\css\help.css" "%docbook_outputdir%\css\help.css"

echo Create HTML files...
%docbook_java_exe% %docbook_java_parameters% -cp %docbook_saxon_jar%;%docbook_saxon_xsl% com.icl.saxon.StyleSheet %docbook_inputfile% %docbook_use_stylesheet% base.dir=%docbook_outputdir%\
if exist "htmlhelp.hhp" goto compile

:compile
echo Compile HTML Help...
%docbook_hhc_exe% "htmlhelp.hhp"
move "htmlhelp.chm" "%docbook_outputdir_final%\WinMerge.chm"

:clean
echo Cleaning...
del "%docbook_outputdir%\*.html"
del "htmlhelp.hhp"
del "toc.hhc"
del "index.hhk"

rem if Windows NT/2000/XP...
if "%OS%" == "Windows_NT" goto cleannt
rem if Windows 9x...
if "%OS%" == "" goto clean9x

:clean9x
deltree /Y "%docbook_outputdir%\images"
deltree /Y "%docbook_outputdir%\screenshots"
deltree /Y "%docbook_outputdir%\art"
deltree /Y "%docbook_outputdir%\css"
if not "%docbook_build_path%" == "." deltree /Y "%docbook_outputdir%"
echo Finished!
goto end

:cleannt
rd /S /Q "%docbook_outputdir%\images"
rd /S /Q "%docbook_outputdir%\screenshots"
rd /S /Q "%docbook_outputdir%\art"
rd /S /Q "%docbook_outputdir%\css"
if not "%docbook_build_path%" == "." rd /S /Q "%docbook_outputdir%"
echo Finished!
goto end

:error
echo ERROR!!!
goto end

:end
@echo on