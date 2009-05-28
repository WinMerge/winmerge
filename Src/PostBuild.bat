@echo off

REM $Id$

REM _ACP_ATLPROV is set by VS2005/VS2008 when compiling in IDE. But it is NOT
REM set when calling devenv from command line.

REM This detects if the script is called from VS2003 or from IDE
if NOT "%_ACP_ATLPROV%" == "" goto IDEBuild

REM This detects if the script is called from VS2005/VS2008 cmd line env
if "%VCBuildHelper_Command%" == "" goto _MSDEV_BLD_ENV_(%_MSDEV_BLD_ENV_%)

:IDEBuild
:_MSDEV_BLD_ENV_(1)
cd
REM Enable echo lines below if you need to debug this script
REM echo %0
REM echo $(IntDir) = %1
REM echo $(TargetPath) = %2
REM Create English.pot and MergeLang.rc from Merge.rc
cd ..\Translations\WinMerge
cscript CreateMasterPotFile.vbs

REM Create MergeLang.dll from MergeLang.rc
rc /fo..\%1\MergeLang.res /i..\..\Src MergeLang.rc
link /DLL /NOENTRY /MACHINE:IX86 /OUT:%2\..\MergeLang.dll ..\%1\MergeLang.res
REM Copy PO files to where WinMerge expects them
mkdir %2\..\Languages
copy *.po %2\..\Languages
exit

:_MSDEV_BLD_ENV_()
echo Merge.dsp/Merge.vcproj post-build script
echo Not intended for direct invocation through user interface
echo Post-build command line: PostBuild.bat $(IntDir) $(TargetPath)
pause
exit
