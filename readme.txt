WinMerge\readme.txt

Source code for the WinMerge program, its plugins, filters, setup program,
and various utilities are all kept in the WinMerge module directory, in 
various subdirectories.

Generally, look for a file "readme.txt" to explain the purpose of a directory,
and a file "Changes.txt" to hold the historical changelog; sometimes the
"Changes.txt" file is located at a higher level -- eg, the WinMerge\Src\Changes.txt
includes all changes to all subdirectories located under WinMerge\Src.

Subdirectories include:

 - Docs
    both user and developer documentation, in different subdirectories

 - Src
    source code to the WinMerge program itself

 - Plugins
    source code and binaries for WinMerge runtime plugin dlls & scripts

 - Filters
    WinMerge file filters which are shipped with the distribution

 - ArchiveSupport
    source code for the Merge7z dlls, which connect WinMerge with 7-Zip
    Also this directory is required to compile WinMerge(U).exe. There
    is also standalone installer for Merge7z dlls.

 - Installer
    Installer for WinMerge

 - Tools
    Various utilities used by WinMerge developers; see readme files in each

 - ShellExtension
   Windows shell (Explorer) integration. Adds menuitems to Explorer context
   menu for comparing files and folders.

 - Testing
    A suite of test diff files and a script to run them and report the results

 - Build
    This (non-cvs) directory is where WinMerge(U).exe and plugin binaries are
    compiled.

 - BuildTmp
    This (non-cvs) directory is for obj and such-like temporary compiler files
    used in compiling binaries.
