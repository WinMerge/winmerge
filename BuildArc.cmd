cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=.\Dist
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%

mkdir "%DISTDIR%\WinIMerge" 2> NUL
mkdir "%DISTDIR%\WinIMerge\bin" 2> NUL
mkdir "%DISTDIR%\WinIMerge\bin64" 2> NUL

copy Build\Release\WinIMerge.exe "%DISTDIR%\WinIMerge\bin"
copy Build\Release\WinIMergeLib.dll "%DISTDIR%\WinIMerge\bin"
copy Build\x64\Release\WinIMerge.exe "%DISTDIR%\WinIMerge\bin64"
copy Build\x64\Release\WinIMergeLib.dll "%DISTDIR%\WinIMerge\bin64"
copy GPL.txt "%DISTDIR%\WinIMerge"
copy freeimage-license-gplv2.txt "%DISTDIR%\WinIMerge"

7z.exe a -tzip "%DISTDIR%\winimerge-%MAJOR%-%MINOR%-%REVISION%-%PATCHLEVEL%-exe.zip" "%DISTDIR%\WinIMerge\"

