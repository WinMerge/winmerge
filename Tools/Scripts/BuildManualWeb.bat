@echo off
rem This batch file calls the script to build the manual as HTML files.

set build_para=withads
if "%1" == "withoutads" set build_para=
if "%1" == "withoutads" goto start
if "%1" == "" goto start

echo "%1" is a invalid argument. Please use "withoutads", if you don't want advertisements in the manual.
goto end

:start
pushd "../../Docs/Users/Manual/build/"
call build_html %build_para%
popd

:end
@echo on