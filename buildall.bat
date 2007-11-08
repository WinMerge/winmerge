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
del /S /Q  BuildTmp

: Remove expat build files
del /S /Q Externals\expat\lib\Release

: Remove pcre build files
del /S /Q Externals\pcre\bin
del /S /Q Externals\pcre\dll_pcre\o
del /S /Q Externals\pcre\lib_pcre\o
del /S /Q Externals\pcre\lib_pcreposix\o
del /S /Q Externals\pcre\pcretest\o

: Remove scew build files
del /S /Q Externals\scew\win32\lib
del /S /Q Externals\scew\win32\obj

: Build WinMerge executables
devenv Src\Merge.dsp /rebuild Release
devenv Src\Merge.dsp /rebuild UnicodeRelease
