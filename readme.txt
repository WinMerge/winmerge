WinMerge\readme.txt

Source code for the WinMerge program, its plugins, filters, setup program,
and various utilities are all kept in various subfolders listed below.

Changelog file is in Docs/Users/ChangeLog.txt and it logs user-visible
or otherwise significant changes. Subversion commit messages provide a
detailed changelog for all the changes. Commit messages also have a
SourceForge.net tracker number mentioned when the commit is related to
one of the tracker items.

Subfolders include:

 - Docs
    Both user and developer documentation, in different subfolders.
    Can be browsed by opening index.html in Docs folder.

 - Src
    source code to the WinMerge program itself

 - Plugins
    source code and binaries for WinMerge runtime plugin dlls & scripts

 - Filters
    WinMerge file filters which are shipped with the distribution

 - ArchiveSupport
    source code for the Merge7z dlls, which connect WinMerge with 7-Zip
    Also this folder is required to compile WinMerge(U).exe. There
    is also standalone installer for Merge7z dlls.

 - Externals
    This folder contains several libraries whose sources come from
    outside WinMerge project. They are stored here for convenience for
    building and possible needed small changes for WinMerge. Libraries
    include XML parser and regular expression parser.

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
    This folder gets created by compiler when WinMerge is compiled. It
    contains compiled executables, libraries, user manual etc.

 - BuildTmp
    This folder gets created by compiler when WinMerge is compiled. It
    contains temporary files created during the compilation and can be safely
    removed.
