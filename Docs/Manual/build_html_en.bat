@echo off

pushd %~dp0
call configuration.bat

set docbook_inputfile=EN\WinMerge_help.xml
set docbook_use_stylesheet=EN\build\build_html.xsl
set docbook_outputdir=%docbook_build_path%\html-en
set ads=false
if "%1" == "withads" goto withads
if not "%1" == "" goto withadserror
goto start

:withads
set ads=true
echo **************************************
echo * The manual includes advertisements *
echo **************************************
goto start

:withadserror
echo "%1" is a invalid argument. Please use "withads", if you want advertisements in the manual.
goto end

:start
if not exist "%docbook_outputdir%" mkdir "%docbook_outputdir%"

echo Copy images ...
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
copy "EN\css\*.css" "%docbook_outputdir%\css"

echo Create Manual HTML files...
%docbook_java_exe% %docbook_java_parameters% -cp %docbook_saxon_jar%;%docbook_xerces_jar%;%docbook_saxon_xsl% -Djavax.xml.parsers.DocumentBuilderFactory=%DBFACTORY% -Djavax.xml.parsers.SAXParserFactory=%SPFACTORY% -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%XINCLUDE% com.icl.saxon.StyleSheet %docbook_inputfile% %docbook_use_stylesheet% base.dir=%docbook_outputdir%\ withads=%ads%

echo Finished!

:end
popd
@echo on