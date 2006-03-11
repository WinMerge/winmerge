@echo off
rem BuildDll.bat
rem
rem This batch file is used to build the Resource DLL
rem Use it like so: BuildDll Bulgarian
rem It returns 1 if there is an error
rem
rem RCS ID line follows -- this is updated by CVS
rem $Id$

if '%1==' echo Syntax: BuildDll language
if '%1==' goto end

echo Building %1
set rcScript=.\%1\Merge%1.rc
set outputdir=..\..\Build\Languages

.\MakeResDll -v -b -o %outputdir% %rcScript%

if errorlevel==1 goto _failed
goto end

:_failed
exit 1

:end
