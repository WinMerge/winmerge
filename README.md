[![logo](Docs/Logos/WinMerge_logo_24bit.png)](https://github.com/WinMerge/winmerge)


# WinMerge
[![Build status](https://ci.appveyor.com/api/projects/status/h3v3ap1kswi1tyyt?svg=true)](https://ci.appveyor.com/project/sdottaka/winmerge/build/artifacts)
[![CI](https://github.com/WinMerge/winmerge/workflows/CI/badge.svg)](https://github.com/WinMerge/winmerge/actions)
[![sourceforge.net downloads](https://img.shields.io/sourceforge/dt/winmerge)](https://sourceforge.net/projects/winmerge/files/)
[![Github Releases All](https://img.shields.io/github/downloads/winmerge/winmerge/total.svg)](https://github.com/WinMerge/winmerge/releases/latest)
[![Translation status](https://img.shields.io/badge/translations-38-green)](https://github.com/WinMerge/winmerge/blob/master/Translations/TranslationsStatus.md)

[WinMerge](https://winmerge.org/) is an open source differencing and merging tool
for Windows. WinMerge can compare files and folders, presenting differences
in a visual format that is easy to understand and manipulate.

## Build Requirements

### Visual Studio 2017

 * *Community*, *Professional* or *Enterprise* Edition
 * VC++ 2017 latest v141 tools
 * Visual C++ compilers and libraries for (ARM, ARM64)
 * Windows XP support for C++
 * Visual C++ MFC for (x86 and x64, ARM, ARM64)
 * Visual C++ ATL for (x86 and x64, ARM, ARM64)
 * Windows 10 SDK

### Visual Studio 2019

 * *Community*, *Professional* or *Enterprise* Edition
 * MSVC v142 - VS 2019 C++ (x64/x86, ARM, ARM64) build tools (Latest)
 * C++ MFC for latest v142 build tools (x86 & x64, ARM, ARM64)
 * C++ ATL for latest v142 build tools (x86 & x64, ARM, ARM64)
 * Windows 10 SDK

### Visual Studio 2022

 * *Community*, *Professional* or *Enterprise* Edition
 * MSVC v143 Buildtools (x64/x86, ARM, ARM64)
 * C++ MFC for latest v143 build tools (x64/x86, ARM, ARM64)
 * C++ ATL for latest v143 build tools (x64/x86, ARM, ARM64)
 * Windows 10 SDK
 
### Other utilities/programs

 * git
 * Inno Setup 5.x and 6.x
 * 7-Zip
 * Python
 * Pandoc
 * MSYS2 and MSYS2 packages (po4a and diffutils)

## How to Build

~~~
git clone --recurse-submodules https://github.com/WinMerge/winmerge
cd winmerge
DownloadDeps.cmd
BuildAll.vs2022.cmd [x86|x64|ARM|ARM64] or BuildAll.vs2019.cmd [x86|x64|ARM|ARM64] or BuildAll.vs2017.cmd [x86|x64|ARM|ARM64]
~~~

## Folder Structure

Source code for the WinMerge program, its plugins, filters, setup program,
and various utilities are all kept in the subfolders listed below.

The changelog file is in `Docs/Users/ChangeLog.md` and it documents 
both user-visible and significant changes.

Subfolders include:

 - `Docs`  
   Both user and developer documentation, in different subfolders.  
   Can be browsed by opening `index.html` in the `Docs` folder.

 - `Src`  
   Source code to the WinMerge program itself.

 - `Plugins`  
   Source code and binaries for WinMerge runtime plugin dlls & scripts.

 - `Filters`  
   WinMerge file filters which are shipped with the distribution.

 - `ArchiveSupport`  
   Source code for the Merge7z dlls, which connect WinMerge with 7-Zip.  
   Also this folder is required to compile `WinMergeU.exe`.  
   There is also a standalone installer for Merge7z dlls.

 - `Externals`  
   This folder contains several libraries whose sources come from
   outside WinMerge project.  
   They are stored here for convenience for building and possibly 
   needed small changes for WinMerge.  
   Libraries include an XML parser and a regular expression parser.

 - `Installer`  
   Installer for WinMerge.

 - `Tools`  
   Various utilities used by WinMerge developers; see readme files in each.

 - `ShellExtension`  
   Windows Shell (Explorer) integration.  
   Adds menuitems to Explorer context menu for comparing files and folders.

 - `Testing`  
   A suite of test diff files and a script to run them and report the results.  
   This folder also has a `Google Test` subfolder containing unit tests made
   with [Google Test Framework](https://github.com/google/googletest).

 - `Build`  
   This folder gets created by the compiler when WinMerge is compiled.  
   It contains compiled executables, libraries, the user manual, etc.

 - `BuildTmp`  
   This folder gets created by the compiler when WinMerge is compiled.  
   It contains temporary files created during the compilation and can be 
   safely deleted. 


## How to CONTRIBUTE

   You will need to fork the main Winmerge repository and create a branch on that fork
   
   Your new code needs to follow [Eric Allman indentation](https://en.wikipedia.org/wiki/Indentation_style#Allman_style)
   
   When your code is ready for a review/merge create a pull request explaining the changes that you made
   

## How to RUN and DEBUG

   The winmerge folder has different Visual Studio solution files (.sln) that you can use to build, debug and run while you test your changes
   
   If you have run any of the BuildAll scripts you can run WinMerge from path `\Build\X64\Release\WinMergeU.exe` 
   if your architecture is not `X64` look for any of the other folders generated after the build has finished
   
   Another way to Debug, run the exe from previous step, then from VS attach to the running process.
