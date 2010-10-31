@echo off

REM $Id$

set info=echo *

REM _ACP_ATLPROV was introduced in VC7. If not set, assume VC6.
REM _MSDEV_BLD_ENV_ was removed in VC8. Don't care about it if _ACP_ATLPROV is set.
REM _ACP_ATLPROV is set by VS2005/VS2008 when compiling in IDE. But it is NOT
REM set when calling devenv from command line.

REM This detects if the script is called from VS2003 or from IDE
if NOT "%_ACP_ATLPROV%" == "" goto IDEBuild

REM This detects if the script is called from VS2005/VS2008 cmd line env
if "%VCBuildHelper_Command%" == "" goto _MSDEV_BLD_ENV_(%_MSDEV_BLD_ENV_%)

:IDEBuild

set msdev=rem
set devenv=devenv
goto Configure

:_MSDEV_BLD_ENV_(1)
set msdev=msdev
set devenv=rem
goto Configure

:_MSDEV_BLD_ENV_()
echo Merge.dsp/Merge.vcproj pre-link script
echo Not intended for direct invocation through user interface
echo Pre-link command line: PreLink.bat $(IntDir) $(TargetPath)
pause
exit

:Configure
%info% Configure

cd
REM Enable echo lines below if you need to debug this script
REM echo %0
REM echo $(IntDir) = %1
REM echo $(TargetPath) = %2
del %2

REM Create build directories
mkdir ..\Build\expat
mkdir ..\Build\pcre
mkdir ..\Build\heksedit
goto %1

:.\..\BuildTmp\MergeUnicodeDebug
%info% UNICODE/Debug
mkdir ..\Build\MergeUnicodeDebug
goto Debug

:.\..\BuildTmp\MergeUnicodeRelease
%info% UNICODE/Release
mkdir ..\Build\MergeUnicodeRelease
goto Release

:Debug
%info% Debug

REM Build expat
cd %2\..\..\..\Externals\expat\lib
%msdev% "expat.dsp" /make "expat - Win32 Debug"
%devenv% "expat.vcproj" /build "Debug"
cd %2\..\..\expat
copy lib\debug\libexpat.dll
copy lib\debug\libexpat.lib
copy libexpat.dll %2\..\

REM Build SCEW
cd %2\..\..\..\Externals\scew\win32
%msdev% "scew.dsp" /make "scew - Win32 Debug"
%devenv% "scew.vcproj" /build "Debug"

REM Build PCRE
cd %2\..\..\..\Externals\pcre\Win32
%msdev% "pcre.dsp" /make "pcre - Win32 Debug"
%devenv% "pcre.vcproj" /build "Debug"
copy Debug\pcre.dll %2\..\..\pcre
copy Debug\pcre.lib %2\..\..\pcre

goto Common

:Release
%info% Release

REM Build expat
cd %2\..\..\..\Externals\expat\lib
%msdev% "expat.dsp" /make "expat - Win32 Release"
%devenv% "expat.vcproj" /build "Release"
cd %2\..\..\expat
copy lib\release\libexpat.dll
copy lib\release\libexpat.lib
copy libexpat.dll %2\..\


REM Build SCEW
cd %2\..\..\..\Externals\scew\win32
%msdev% "scew.dsp" /make "scew - Win32 Release"
%devenv% "scew.vcproj" /build "Release"


REM Build PCRE
cd %2\..\..\..\Externals\pcre\Win32
%msdev% "pcre.dsp" /make "pcre - Win32 Release"
%devenv% "pcre.vcproj" /build "MinSizeRel"
copy MinSizeRel\pcre.dll %2\..\..\pcre
copy MinSizeRel\pcre.lib %2\..\..\pcre

goto Common

:Common
%info% Common
