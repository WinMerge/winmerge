cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=\Web Page\geocities
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%

copy "Build\WinMerge-%RCVER%-Setup.exe" "WinMerge-%APPVER%-Setup.exe"
copy "Build\WinMerge-%RCVER%-Setup-vc71.exe" "WinMerge-%APPVER%-Setup-vc71.exe"
copy "Build\WinMerge-%RCVER%-x64-Setup.exe" "WinMerge-%APPVER%-x64-Setup.exe"
7z.exe a -tzip "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe.zip" "WinMerge-%APPVER%-Setup.exe"
7z.exe a -tzip "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup-vc71.exe.zip" "WinMerge-%APPVER%-Setup-vc71.exe"
7z.exe a -tzip "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe.zip" "WinMerge-%APPVER%-x64-Setup.exe"

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version-vc71\WinMerge\" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Languages" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version-vc71\WinMerge\Languages" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Languages" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Filters" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version-vc71\WinMerge\Filters" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version-vc71\WinMerge\MergePlugins" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Docs" 2> NUL
mkdir "%DISTDIR%\2.14-zip-version-vc71\WinMerge\Docs" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs" 2> NUL

rem Readme
for %%i in (Build\Docs\*.*) do (
  copy "%%i" "%DISTDIR%\2.14-zip-version\WinMerge\%%~nxi"
  copy "%%i" "%DISTDIR%\2.14-zip-version-vc71\WinMerge\%%~nxi"
  copy "%%i" "%DISTDIR%\2.14-x64-zip-version\WinMerge\%%~nxi"
)

rem Help
copy Build\Manual\htmlhelp\WinMerge.chm "%DISTDIR%\2.14-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge_ja.chm "%DISTDIR%\2.14-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge.chm "%DISTDIR%\2.14-zip-version-vc71\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge_ja.chm "%DISTDIR%\2.14-zip-version-vc71\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge.chm "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge_ja.chm "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs"

rem Excecutables
copy Build\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\vc71\MergeRelease\WinMerge.exe "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy Build\vc71\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy Build\X64\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem ShellExtension
copy "Build\ShellExtensionUnicode Release mindependency\ShellExtensionU.dll" "%DISTDIR%\2.14-zip-version\WinMerge\"
copy "Build\vc71\ShellExtensionRelease mindependency\ShellExtension.dll" "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy "Build\vc71\ShellExtensionUnicode Release mindependency\ShellExtensionU.dll" "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy "Build\X64\ShellExtensionUnicode Release mindependency\ShellExtensionX64.dll" "%DISTDIR%\2.14-zip-version\WinMerge\"
copy "Build\X64\ShellExtensionUnicode Release mindependency\ShellExtensionX64.dll" "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy "Build\X64\ShellExtensionUnicode Release mindependency\ShellExtensionX64.dll" "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy ShellExtension\*.bat "%DISTDIR%\2.14-zip-version\WinMerge\"
copy ShellExtension\*.bat "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy ShellExtension\*.bat "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Dlls
copy Build\pcre\minsizerel\pcre.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\expat\lib\release\libexpat.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\vc71\pcre\minsizerel\pcre.dll "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy Build\vc71\expat\release\libexpat.dll "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy Build\pcre\X64\minsizerel\pcre.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy Build\expat\lib\X64\release\libexpat.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Translation
copy Build\MergeUnicodeRelease\Languages\Japanese.po "%DISTDIR%\2.14-zip-version\WinMerge\Languages\"
copy Build\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\vc71\MergeUnicodeRelease\Languages\Japanese.po "%DISTDIR%\2.14-zip-version\WinMerge\Languages\"
copy Build\vc71\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\Languages\Japanese.po "%DISTDIR%\2.14-x64-zip-version\WinMerge\Languages\"
copy Build\X64\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Filters
for %%i in (Build\Filters\*.flt Build\Filters\*.tmpl Build\Filters\*.txt) do (
  copy "%%i" "%DISTDIR%\2.14-zip-version\WinMerge\Filters\%%~nxi"
  copy "%%i" "%DISTDIR%\2.14-zip-version-vc71\WinMerge\Filters\%%~nxi"
  copy "%%i" "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters\%%~nxi"
)

rem Plugins
copy Plugins\dlls\*.dll "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.dll "%DISTDIR%\2.14-zip-version-vc71\WinMerge\MergePlugins\"
copy Plugins\dlls\X64\*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\2.14-zip-versio-vc71\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\"
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\CompareMS*.sct"
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"
del "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"

rem Runtimes
copy Installer\Runtimes\*.dll "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy "c:\Program Files (x86)\Microsoft Visual Studio 10.0\vc\redist\x86\Microsoft.VC100.CRT\*.*" "%DISTDIR%\2.14-zip-version\WinMerge\
copy "c:\Program Files (x86)\Microsoft Visual Studio 10.0\vc\redist\x86\Microsoft.VC100.MFC\*u.*" "%DISTDIR%\2.14-zip-version\WinMerge\
copy "c:\Program Files (x86)\Microsoft Visual Studio 10.0\vc\redist\x86\Microsoft.VC100.MFCLOC\mfc100jpn.dll" "%DISTDIR%\2.14-zip-version\WinMerge\
copy "c:\Program Files (x86)\Microsoft Visual Studio 10.0\vc\redist\x64\Microsoft.VC100.CRT\*.*" "%DISTDIR%\2.14-x64-zip-version\WinMerge\
copy "c:\Program Files (x86)\Microsoft Visual Studio 10.0\vc\redist\x64\Microsoft.VC100.MFC\*u.*" "%DISTDIR%\2.14-x64-zip-version\WinMerge\
copy "c:\Program Files (x86)\Microsoft Visual Studio 10.0\vc\redist\x64\Microsoft.VC100.MFCLOC\mfc100jpn.dll" "%DISTDIR%\2.14-zip-version\WinMerge\

rem 7zPlugins
copy Build\vc71\MergeRelease\Merge7z*.dll "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy Build\MergeUnicodeRelease\Merge7z*.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\vc71\MergeUnicodeRelease\Merge7z*.dll "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
copy Build\X64\MergeUnicodeRelease\Merge7z*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"


7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip" "%DISTDIR%\2.14-zip-version\WinMerge\"
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-exe-vc71.zip" "%DISTDIR%\2.14-zip-version-vc71\WinMerge\"
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip" "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"

7z.exe a -t7z  -xr!*.bak -xr!*.lang -xr!*.ncb  -xr!*.sdf -xr!*.bsc -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.idb -xr!*.%COMPUTERNAME%.%USERNAME%.user -xr!BuildLog.htm -xr!CVS -xr!.svn -xr!Brazilian.po -xr!Bulgarian.po -xr!Catalan.po -xr!Croatian.po -xr!ChineseSimplified.po -xr!ChineseTraditional.po -xr!CVS -xr!Czech.po -xr!Danish.po -xr!Dutch.po -xr!French.po -xr!German.po -xr!Hungarian.po -xr!Italian.po -xr!Korean.po -xr!Norwegian.po -xr!Polish.po -xr!Portuguese.po -xr!Russian.po -xr!Slovak.po -xr!Spanish.po -xr!Swedish.po -xr!Turkish.po "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Web readme.txt Version.h *.cmd *.bat *.inf *.sln

explorer "%DISTDIR%"
pause
