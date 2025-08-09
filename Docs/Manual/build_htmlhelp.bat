@echo off

setlocal enabledelayedexpansion
pushd "%~dp0"
call configuration.bat

if "%1"=="" (
  echo Error: Missing arguments.
  goto end
)

set docbook_inputfile=%1\Index.xml
set docbook_use_stylesheet=%1\xsl\htmlhelp.xsl
set docbook_outputdir=htmlhelp
set docbook_outputdir_final=%docbook_build_path%\%docbook_outputdir%

if not exist "%docbook_outputdir%" mkdir "%docbook_outputdir%"
if not exist "%docbook_outputdir_final%" mkdir "%docbook_outputdir_final%"

if "%4" == "/build" (
  if exist "%docbook_outputdir_final%\%2" (
    copy "%docbook_outputdir_final%\%2" %1\%2 2> NUL > NUL
  )
  for /f "tokens=*" %%i in ('dir /a:-d /b /o:d /t:w %1') do set NEWEST=%%~nxi
  del %1\%2 2> NUL
  if "!NEWEST!" == "%2" goto end
) else if "%4" == "/clean" (
  del "%docbook_outputdir_final%\%2"
  goto end
)

echo Copy images...
if not exist "%docbook_outputdir%\images" mkdir "%docbook_outputdir%\images"
copy "Shared\images\*.gif" "%docbook_outputdir%\images\."
copy "Shared\images\*.png" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "%1\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "Shared\css\*.css" "%docbook_outputdir%\css\."

echo Create HTML files...
%docbook_xsltproc% --xinclude --nonet --stringparam base.dir %docbook_outputdir%/ --path .. %docbook_use_stylesheet% %docbook_inputfile%
if exist "htmlhelp.hhp" goto compile

:compile
echo Compile HTML Help...
Tools\SBAppLocale.exe %3 %docbook_hhc_exe% "htmlhelp.hhp"
move "htmlhelp.chm" "%docbook_outputdir_final%\%2"

:clean
echo Cleaning...
del "%docbook_outputdir%\*.html"
del "htmlhelp.hhp"
del "toc.hhc"
del "index.hhk"
rd /S /Q "%docbook_outputdir%\images"
rd /S /Q "%docbook_outputdir%\screenshots"
rd /S /Q "%docbook_outputdir%\css"
if not "%docbook_build_path%" == "." rd /S /Q "%docbook_outputdir%"
echo Finished!
goto end

:error
echo ERROR!!!
goto end

:end
popd
setlocal disabledelayedexpansion
@echo on
