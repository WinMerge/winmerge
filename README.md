[![logo](Docs/Logos/WinMerge_logo_24bit.png)](https://github.com/WinMerge/winmerge)


# WinMerge Source Code
[![Build status](https://ci.appveyor.com/api/projects/status/h3v3ap1kswi1tyyt?svg=true)](https://ci.appveyor.com/project/sdottaka/winmerge/build/artifacts)
[![CI](https://github.com/WinMerge/winmerge/workflows/CI/badge.svg)](https://github.com/WinMerge/winmerge/actions)
[![sourceforge.net downloads](https://img.shields.io/sourceforge/dt/winmerge)](https://sourceforge.net/projects/winmerge/files/)
[![Github Releases All](https://img.shields.io/github/downloads/winmerge/winmerge/total.svg)](https://github.com/WinMerge/winmerge/releases/latest)
[![Translation status](https://img.shields.io/badge/translations-35-green)](https://github.com/WinMerge/winmerge/blob/master/Translations/TranslationsStatus.md)

[WinMerge](https://winmerge.org/) is an open source differencing and merging tool
for Windows. WinMerge can compare files and folders, presenting differences
in a visual format that is easy to understand and manipulate.

## Requirements

### Visual Studio 2017

 * *Community*, *Professional* or *Enterprise* Edition
 * VC++ 2017 v141 toolset
 * Windows XP support for C++
 * MFC and ATL support
 * Windows 10 SDK

### Visual Studio 2019

 * *Community*, *Professional* or *Enterprise* Edition
 * MSVC v142 Buildtools
 * C++ MFC for v142 Buildtools
 * Windows 10 SDK

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
