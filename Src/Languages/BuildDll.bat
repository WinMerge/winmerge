@echo off
rem This batch file is used to build the Resource DLL
if '%1==' echo Syntax: BuildDll language
if '%1==' goto end
echo Building %1
set rcScript=.\%1\Merge%1.rc
set outputdir=.\DLL

.\MakeResDll -b -o %outputdir% %rcScript%

:end
