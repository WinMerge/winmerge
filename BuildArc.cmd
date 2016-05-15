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

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\frhed" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\frhed\Docs" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\Docs" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\frhed\Languages" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\Languages" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\WinIMerge" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\WinIMerge" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Merge7z" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Merge7z" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\Merge7z\Lang" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\Merge7z\Lang" 2> NUL

mkdir "%DISTDIR%\2.14-zip-version\WinMerge\GnuWin32" 2> NUL
mkdir "%DISTDIR%\2.14-x64-zip-version\WinMerge\GnuWin32" 2> NUL

rem Readme
for %%i in (Translations\Docs\Readme\ReadMe-Japanese.txt) do (
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
copy ShellExtension\*Register.bat "%DISTDIR%\2.14-zip-version\WinMerge\"
copy ShellExtension\*Register.bat "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

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
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\CompareMS*.dll"
del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"
rem del "%DISTDIR%\2.14-zip-version\WinMerge\MergePlugins\DisplayXMLFiles.dll"
del "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"
rem del "%DISTDIR%\2.14-x64-zip-version\WinMerge\MergePlugins\DisplayXMLFiles.dll"

rem 7zPlugins
copy Build\MergeUnicodeRelease\Merge7z\Merge7z*.dll "%DISTDIR%\2.14-zip-version\WinMerge\Merge7z\"
copy Build\MergeUnicodeRelease\Merge7z\7z.dll "%DISTDIR%\2.14-zip-version\WinMerge\Merge7z\"
copy Build\MergeUnicodeRelease\Merge7z\*.txt "%DISTDIR%\2.14-zip-version\WinMerge\Merge7z\"
copy Build\MergeUnicodeRelease\Merge7z\Lang\*.txt "%DISTDIR%\2.14-zip-version\WinMerge\Merge7z\Lang\"
copy Build\X64\MergeUnicodeRelease\Merge7z\Merge7z*.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\Merge7z\"
copy Build\X64\MergeUnicodeRelease\Merge7z\7z.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\Merge7z\"
copy Build\X64\MergeUnicodeRelease\Merge7z\*.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\Merge7z\"
copy Build\X64\MergeUnicodeRelease\Merge7z\Lang\*.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\Merge7z\Lang\"

rem Frhed
copy Externals\Frhed\GPL.txt "%DISTDIR%\2.14-zip-version\WinMerge\frhed\"
rem copy Externals\Frhed\Build\FRHED_vc10\Win32\UnicodeRelease\frhed.exe "%DISTDIR%\2.14-zip-version\WinMerge\frhed\"
copy Externals\Frhed\Build\FRHED_vc10\Win32\UnicodeRelease\hekseditU.dll "%DISTDIR%\2.14-zip-version\WinMerge\frhed\"
copy Externals\Frhed\Build\FRHED_vc10\Win32\UnicodeRelease\Docs\*.txt "%DISTDIR%\2.14-zip-version\WinMerge\frhed\Docs"
copy Externals\Frhed\Build\FRHED_vc10\Win32\UnicodeRelease\Docs\Sample.tpl "%DISTDIR%\2.14-zip-version\WinMerge\frhed\Docs"
copy Externals\Frhed\Build\FRHED_vc10\Win32\UnicodeRelease\Languages\*.po "%DISTDIR%\2.14-zip-version\WinMerge\frhed\Languages"
copy Externals\Frhed\Build\FRHED_vc10\Win32\UnicodeRelease\Languages\heksedit.lng "%DISTDIR%\2.14-zip-version\WinMerge\frhed\Languages"
copy Externals\Frhed\GPL.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\"
rem copy Externals\Frhed\Build\FRHED_vc10\x64\UnicodeRelease\frhed.exe "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\"
copy Externals\Frhed\Build\FRHED_vc10\x64\UnicodeRelease\hekseditU.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\"
copy Externals\Frhed\Build\FRHED_vc10\x64\UnicodeRelease\Docs\*.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\Docs"
copy Externals\Frhed\Build\FRHED_vc10\x64\UnicodeRelease\Docs\Sample.tpl "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\Docs"
copy Externals\Frhed\Build\FRHED_vc10\x64\UnicodeRelease\Languages\*.po "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\Languages"
copy Externals\Frhed\Build\FRHED_vc10\x64\UnicodeRelease\Languages\heksedit.lng "%DISTDIR%\2.14-x64-zip-version\WinMerge\frhed\Languages"

rem WinIMerge
copy Externals\WinIMerge\GPL.txt "%DISTDIR%\2.14-zip-version\WinMerge\WinIMerge\"
copy Externals\WinIMerge\freeimage-license-gplv2.txt "%DISTDIR%\2.14-zip-version\WinMerge\WinIMerge\"
rem copy Externals\WinIMerge\Build\Release\WinIMerge.exe "%DISTDIR%\2.14-zip-version\WinMerge\WinIMerge\"
copy Externals\WinIMerge\Build\Release\WinIMergeLib.dll "%DISTDIR%\2.14-zip-version\WinMerge\WinIMerge\"
copy "%VS140COMNTOOLS%\..\..\VC\redist\x86\Microsoft.VC140.OpenMP\vcomp140.dll" "%DISTDIR%\2.14-zip-version\WinMerge\
copy Externals\WinIMerge\GPL.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\WinIMerge\"
copy Externals\WinIMerge\freeimage-license-gplv2.txt "%DISTDIR%\2.14-x64-zip-version\WinMerge\WinIMerge\"
rem copy Externals\WinIMerge\Build\x64\Release\WinIMerge.exe "%DISTDIR%\2.14-x64-zip-version\WinMerge\WinIMerge\"
copy Externals\WinIMerge\Build\x64\Release\WinIMergeLib.dll "%DISTDIR%\2.14-x64-zip-version\WinMerge\WinIMerge\"
copy "%VS140COMNTOOLS%\..\..\VC\redist\x64\Microsoft.VC140.OpenMP\vcomp140.dll" "%DISTDIR%\2.14-x64-zip-version\WinMerge\

rem Patch
xcopy /s/y "C:\Program Files\WinMerge\GnuWin32" "%DISTDIR%\2.14-zip-version\WinMerge\GnuWin32\"
xcopy /s/y "C:\Program Files\WinMerge\GnuWin32" "%DISTDIR%\2.14-x64-zip-version\WinMerge\GnuWin32\"

7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip" "%DISTDIR%\2.14-zip-version\WinMerge\"
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip" "%DISTDIR%\2.14-x64-zip-version\WinMerge\"

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"

7z.exe a -t7z -xr!*.gcno -xr!*.gcda -xr!*.gcov -xr!*.orig -xr!*.o -xr!*.a -xr!.dep -xr!*.out -xr!debug_static* -xr!debug_shared* -xr!release_static* -xr!release_shared* -xr!*.bak -xr!*.lang -xr!*.db -xr!*.ncb  -xr!*.sdf -xr!*.bsc -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.ilk -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.exp -xr!*.idb -xr!*.%COMPUTERNAME%.%USERNAME%.user -xr!BuildLog.htm -xr!CVS -xr!.svn -xr!ipch -xr!*.tlb -xr!*.tlog -xr!*.un~ -xr!CVS -xr!.hg -xr!*.log -xr!*.lastbuildstate -xr!FreeImage*.dll -xr!WinIMergeLib.dll -xr!WinIMerge.exe -xr!BuildTmp "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Translations Web readme.txt Version.h *.cmd *.bat *.inf *.sln *.vbs

(
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe.zip"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"
) > "%DISTDIR%\jp_files.txt"

pause
