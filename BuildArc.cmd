cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=\Web Page\geocities
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%

copy "Build\WinMerge-%RCVER%-Setup.exe" "WinMerge-%APPVER%-Setup.exe"
copy "Build\WinMerge-%RCVER%-x64-Setup.exe" "WinMerge-%APPVER%-x64-Setup.exe"
7z.exe a -tzip "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe.zip" "WinMerge-%APPVER%-Setup.exe"
7z.exe a -tzip "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe.zip" "WinMerge-%APPVER%-x64-Setup.exe"

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Languages" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Languages" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Filters" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Docs" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs" 2> NUL

rem Readme
for %%i in (Build\Docs\*.*) do (
  copy "%%i" "%DISTDIR%\2.14-zip-version\WinMerge\%%~nxi"
  copy "%%i" "%DISTDIR%\2.14-x64-zip-version\WinMerge\%%~nxi"
)

rem Help
copy Build\Manual\htmlhelp\WinMerge.chm "%DISTDIR%\2.14-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge_ja.chm "%DISTDIR%\2.14-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge.chm "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs"
copy Build\Manual\htmlhelp\WinMerge_ja.chm "%DISTDIR%\2.14-x64-zip-version\WinMerge\Docs"

rem Excecutables
copy Build\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy Plugins\WinMerge32BitPluginProxy\Release\WinMerge32BitPluginProxy.exe "%DISTDIR%\2.14-x64-zip-version\WinMerge\

rem ShellExtension
copy "Build\ShellExtensionUnicode Release mindependency\ShellExtensionU.dll" "%DISTDIR%\2.14-zip-version\WinMerge\"
copy "Build\X64\ShellExtensionUnicode Release mindependency\ShellExtensionX64.dll" "%DISTDIR%\2.14-zip-version\WinMerge\"
copy "Build\X64\ShellExtensionUnicode Release mindependency\ShellExtensionX64.dll" "%DISTDIR%\2.14-x64-zip-version\WinMerge\"
copy ShellExtension\*.bat "%DISTDIR%\2.14-zip-version\WinMerge\"
copy ShellExtension\*.bat "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Translation
copy Build\MergeUnicodeRelease\Languages\Japanese.po "%DISTDIR%\2.14-zip-version\WinMerge\Languages\"
copy Build\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\Languages\Japanese.po "%DISTDIR%\2.14-x64-zip-version\WinMerge\Languages\"
copy Build\X64\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

rem Filters
for %%i in (Build\Filters\*.flt Build\Filters\*.tmpl Build\Filters\*.txt) do (
  copy "%%i" "%DISTDIR%\2.14-zip-version\WinMerge\Filters\%%~nxi"
  copy "%%i" "%DISTDIR%\2.14-x64-zip-version\WinMerge\Filters\%%~nxi"
)

rem Plugins
copy Plugins\dlls\*.dll "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\X64\*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\"
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\CompareMS*.sct"
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"
del "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"

rem 7zPlugins
copy Build\MergeUnicodeRelease\Merge7z*.dll "%DISTDIR%\2.14-zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\Merge7z*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\"


7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip" "%DISTDIR%\2.14-zip-version\WinMerge\"
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip" "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"

7z.exe a -t7z -xr!*.o -xr!*.a -xr!.dep -xr!*.out -xr!debug_static* -xr!debug_shared* -xr!release_static* -xr!release_shared* -xr!*.bak -xr!*.lang -xr!*.ncb  -xr!*.sdf -xr!*.bsc -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.ilk -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.idb -xr!*.%COMPUTERNAME%.%USERNAME%.user -xr!BuildLog.htm -xr!CVS -xr!.svn -xr!ipch -xr!*.tlb -xr!*.tlog -xr!CVS "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Translations Web readme.txt Version.h *.cmd *.bat *.inf *.sln

explorer "%DISTDIR%"
pause
