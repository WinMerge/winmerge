@echo off

pushd %~dp0
call configuration.bat

set docbook_inputfile=Hebrew\Index.xml
set docbook_use_stylesheet=Hebrew\xsl\html.xsl
set docbook_outputdir=%docbook_build_path%\html-he
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
copy "Shared\images\*.gif" "%docbook_outputdir%\images\."
copy "Shared\images\*.png" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "Hebrew\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "Shared\css\*.css" "%docbook_outputdir%\css"

echo Create Manual HTML files...
%docbook_xsltproc% --xinclude --nonet --stringparam base.dir %docbook_outputdir%/ --path .. %docbook_use_stylesheet% %docbook_inputfile%

echo Finished!

:end
popd
@echo on