cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=\Web Page\geocities
set path=c:\progra~1\7-zip\;%path%

copy "Build\WinMerge-%RCVER%-Setup.exe" "WinMerge-%APPVER%-Setup.exe"
7z.exe a -tzip "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe.zip" "WinMerge-%APPVER%-Setup.exe"
copy "Build\WinMerge-%RCVER%-x64-Setup.exe" "WinMerge-%APPVER%-x64-Setup.exe"
7z.exe a -tzip "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe.zip" "WinMerge-%APPVER%-x64-Setup.exe"

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Languages" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Filters" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Docs" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Languages" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Microsoft.VC90.CRT" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Microsoft.VC90.MFC" 2> NUL

rem Readme
copy Docs\Users\*.txt "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Src\COPYING "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Docs\Users\*.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy Src\COPYING "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Help
copy Build\Manual\htmlhelp\WinMerge.chm "%DISTDIR%\2.14-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge_ja.chm "%DISTDIR%\2.14-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge.chm "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge_ja.chm "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs"

rem Excecutables
copy Build\MergeRelease\WinMerge.exe "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\X64\MergeRelease\WinMerge.exe "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem ShellExtension
copy Build\MergeRelease\ShellExtension.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\MergeUnicodeRelease\ShellExtensionU.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\ShellExtensionX64\ShellExtensionX64.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy ShellExtension\*.bat "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\ShellExtensionX64\ShellExtensionX64.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy ShellExtension\*.bat "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Dlls
copy Build\pcre\minsizerel\pcre.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\expat\lib\release\libexpat.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\heksedit\release\heksedit.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\pcre\X64\minsizerel\pcre.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy Build\expat\lib\X64\release\libexpat.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy Build\heksedit\X64\release\heksedit.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Translation
copy Build\MergeUnicodeRelease\Languages\Japanese.po "%DISTDIR%\2.14-zip-version\WinMerge\Languages\"
copy Build\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\Languages\Japanese.po "%DISTDIR%\2.14-x64-zip-version\WinMerge\Languages\"
copy Build\X64\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Filters
copy Filters\*.flt "%DISTDIR%\2.14-zip-version\WinMerge\Filters\"
copy Filters\*.tmpl "%DISTDIR%\2.14-zip-version\WinMerge\Filters\"
copy Filters\*.txt "%DISTDIR%\2.14-zip-version\WinMerge\Filters\"
copy Filters\*.flt "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters\"
copy Filters\*.tmpl "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters\"
copy Filters\*.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters\"

rem Plugins
copy Plugins\dlls\*.dll "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\"
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\CompareMS*.sct"
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"
copy Plugins\dlls\X64\*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\"
del "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"

rem Runtimes
copy Installer\Runtimes\*.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy "d:\Program Files\Microsoft Visual Studio 9.0\vc\redist\amd64\Microsoft.VC90.CRT\*.*" "%DISTDIR%\2.14-x64-zip-version\WinMerge\Microsoft.VC90.CRT\"
copy "d:\Program Files\Microsoft Visual Studio 9.0\vc\redist\amd64\Microsoft.VC90.MFC\*.*" "%DISTDIR%\2.14-x64-zip-version\WinMerge\Microsoft.VC90.MFC\"

rem 7zPlugins
copy Build\MergeRelease\Merge7z*.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\MergeUnicodeRelease\Merge7z*.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\X64\MergeRelease\Merge7z*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\Merge7z*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"


7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip" "%DISTDIR%\2.14-zip-version\WinMerge\"
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip" "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"

7z.exe a -t7z  -xr!*.bak -xr!*.lang -xr!*.ncb -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.ilk -xr!*.bsc -xr!*.exp -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.idb -xr!*.R61.taka.user -xr!BuildLog.htm -xr!CVS -xr!.svn "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Translations Web readme.txt Version.h *.cmd *.bat *.inf *.sln

explorer "%DISTDIR%"
pause
