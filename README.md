# WinMerge Source Code

[WinMerge](http://winmerge.org/) is an Open Source differencing and merging tool
for Windows. WinMerge can compare both folders and files, presenting differences
in a visual text format that is easy to understand and handle.

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
and various utilities are all kept in various subfolders listed below.

Changelog file is in `Docs/Users/ChangeLog.txt` and it logs user-visible
or otherwise significant changes.

Subfolders include:

 - `Docs`  
   Both user and developer documentation, in different subfolders.
   Can be browsed by opening `index.html` in `Docs` folder.

 - `Src`  
   Source code to the WinMerge program itself.

 - `Plugins`  
   Source code and binaries for WinMerge runtime plugin dlls & scripts

 - `Filters`  
   WinMerge file filters which are shipped with the distribution.

 - `ArchiveSupport`  
   Source code for the Merge7z dlls, which connect WinMerge with 7-Zip
   Also this folder is required to compile `WinMergeU.exe`. There
   is also standalone installer for Merge7z dlls.

 - `Externals`  
   This folder contains several libraries whose sources come from
   outside WinMerge project. They are stored here for convenience for
   building and possible needed small changes for WinMerge. Libraries
   include XML parser and regular expression parser.

 - `Installer`  
   Installer for WinMerge.

 - `Tools`  
   Various utilities used by WinMerge developers; see readme files in each.

 - `ShellExtension`  
   Windows Shell (Explorer) integration. Adds menuitems to Explorer context
   menu for comparing files and folders.

 - `Testing`  
   A suite of test diff files and a script to run them and report the results.
   This folder also has `Google Test` subfolder containing unit tests made
   with [Google Test Framework](https://github.com/google/googletest).

 - `Build`  
   This folder gets created by compiler when WinMerge is compiled. It
   contains compiled executables, libraries, user manual etc.

 - `BuildTmp`  
   This folder gets created by compiler when WinMerge is compiled. It
   contains temporary files created during the compilation and can be safely
   removed.
