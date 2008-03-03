: WinMerge build "script" using Visual Studio commandline to build
: all needed WinMerge files. This batch must be run from Visual Studio
: command prompt or vcvars32.bat must be run first!

: $Id: $

@echo off

: Projects contain proper dependencies, so all needed projects are
: compiled by compiling the main project. Also, WinMerge project file
: calls Src/PreLink.bat and Src/PostBuild.bat to compile all dependent
: libraries and copy files to Build folder.

: Remove temp files
: expat, scew and pcre also now use BuildTmp
del /S /Q  BuildTmp

: Build WinMerge executables
devenv Src\Merge.dsp /rebuild Release
devenv Src\Merge.dsp /rebuild UnicodeRelease
