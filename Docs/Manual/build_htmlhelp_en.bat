@echo off

pushd %~dp0
call configuration.bat

set docbook_inputfile=EN\Index.xml
set docbook_use_stylesheet=EN\xsl\htmlhelp.xsl
set docbook_outputdir=htmlhelp
set docbook_outputdir_final=%docbook_build_path%\%docbook_outputdir%

if not exist "%docbook_outputdir%" mkdir "%docbook_outputdir%"
if not exist "%docbook_outputdir_final%" mkdir "%docbook_outputdir_final%"

echo Copy images...
if not exist "%docbook_outputdir%\images" mkdir "%docbook_outputdir%\images"
copy "EN\images\*.gif" "%docbook_outputdir%\images\."
copy "EN\images\*.png" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "EN\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy art...
if not exist "%docbook_outputdir%\art" mkdir "%docbook_outputdir%\art"
copy "EN\art\*.*" "%docbook_outputdir%\art\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "EN\css\*.css" "%docbook_outputdir%\css\."

echo Create HTML files...
%docbook_java_exe% %docbook_java_parameters% -cp %docbook_saxon_jar%;%docbook_xerces_jar%;%docbook_saxon_xsl% -Djavax.xml.parsers.DocumentBuilderFactory=%DBFACTORY% -Djavax.xml.parsers.SAXParserFactory=%SPFACTORY% -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%XINCLUDE% com.icl.saxon.StyleSheet %docbook_inputfile% %docbook_use_stylesheet% base.dir=%docbook_outputdir%\
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
popd
@echo on
