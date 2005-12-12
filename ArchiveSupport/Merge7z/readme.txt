Merge7z readme
==============

The Merge7z DLL files provide WinMerge its seamless handling of compressed files. 
For WinMerge to be able to open compressed files, it requires a Merge7z DLL file, 
(for example, Merge7z430.dll), through which it calls 7-Zip archive code.

7-Zip is a Free and Open Source program handling many compression formats,
available from:
   http://www.7-zip.org/

The Merge7z DLL files are available in two forms:

(1) A Windows installer is available, which puts the files into the
correct place for you. For example, "Merge7zInstaller0017-311-430.exe"
is a Merge7z installer which will install Merge7z DLL files which are 
compatible with 7-Zip releases 3.11 through 4.30. (The 0017 indicates
that this is build 0017 of the installer.)

(2) A simple binary bundle is available, from which you should copy
all files into the same directory as your WinMerge.exe and WinMergeU.exe
programs (typically this would be C:\Program Files\WinMerge).
Merge7z0017-311-430.7z is an example of a binary bundle distribution.
This binary bundle is so named because it is build 0017 and contains DLLs 
compatible with 7-Zip versions 3.11 through 4.30.



The WinMerge User's Manual contains a chapter on 7-Zip and archive support.
  http://winmerge.org/2.4/manual/installing.html

Configuration information about your current system, and its presence or
absence of archive support, is available from the WinMerge Help/Configuration
menu, which will create a long text file containing configuration information.
The section beginning "Archive support" has details about Merge7z and
7-Zip files discovered by WinMerge.
