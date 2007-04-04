: WinMerge build "script" using devenv commandline to build
: all needed WinMerge files. This batch must be run from Visual Studio
: command prompt or vcvars32.bat must be run first!

: Building language files requires you have compiled language compiler
: from Tools\MakeResDll and copied the MakeResDll.exe from
: Build\mergerelease to Src\Languages folder.

: !!!NOTENOTE!!!
: If you are using VS2003.Net or VS2005, you'll need to convert project
: files to new format before this batch file can be used. Converting is
: done when the project file is first time opened in Visual Studio.
: So you'll need to open these project files once to get them converted:
: Externals\expat\lib\expat.dsp
: Externals\scew\win32\scew.dsp
: Externals\pcre\dll_pcre\pcre.dsp
: Src\Merge.dsp

@echo off

: Build dependencies first in this order:
: 1. expat
: 2. SCEW
: 3. PCRE
: Note that expat MUST be compiled before SCEW!
:
: Projects contain proper dependencies, so all needed projects are
: compiled by compiling the main project.

: Build expat
devenv Externals\expat\lib\expat.dsp /rebuild Release

: Build SCEW
devenv Externals\scew\win32\scew.dsp /rebuild Release

: Build PCRE
devenv Externals\pcre\dll_pcre\pcre.dsp /rebuild Release

: Build WinMerge executables
devenv Src\Merge.dsp /rebuild Release
devenv Src\Merge.dsp /rebuild UnicodeRelease

: Build language files
call Src\Languages\BuildAll.bat
