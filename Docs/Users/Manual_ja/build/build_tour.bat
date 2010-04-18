@echo off

call configuration.bat

set docbook_inputfile=..\tour\WinMerge_tour.xml
set docbook_use_stylesheet=build_tour.xsl
set docbook_outputdir=%docbook_build_path%\tour
set docbook_outputfile=index.html
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
copy "..\images\*.gif" "%docbook_outputdir%\images\."
copy "..\images\*.png" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "..\tour\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "..\css\*.css" "%docbook_outputdir%\css"

echo Create Tour HTML file...
%docbook_java_exe% %docbook_java_parameters% -cp %docbook_saxon_jar%;%docbook_xerces_jar%;%docbook_saxon_xsl% -Djavax.xml.parsers.DocumentBuilderFactory=%DBFACTORY% -Djavax.xml.parsers.SAXParserFactory=%SPFACTORY% -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%XINCLUDE% com.icl.saxon.StyleSheet -o %docbook_outputdir%/%docbook_outputfile% %docbook_inputfile% %docbook_use_stylesheet% withads=%ads%"

echo Finished!

:end
@echo on