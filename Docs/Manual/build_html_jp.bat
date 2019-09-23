@echo off

pushd %~dp0
call configuration.bat

set docbook_inputfile=JP\Index.xml
set docbook_use_stylesheet=JP\xsl\html.xsl
set docbook_outputdir=%docbook_build_path%\html-jp
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
copy "JP\images\*.gif" "%docbook_outputdir%\images\."
copy "JP\images\*.png" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "JP\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy art...
if not exist "%docbook_outputdir%\art" mkdir "%docbook_outputdir%\art"
copy "JP\art\*.*" "%docbook_outputdir%\art\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "JP\css\*.css" "%docbook_outputdir%\css"

echo Create Manual HTML files...
%docbook_xsltproc% --xinclude --nonet --stringparam base.dir %docbook_outputdir%/ --path .. %docbook_use_stylesheet% %docbook_inputfile%

echo Finished!

:end
popd
@echo on