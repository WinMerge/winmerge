pushd "%~dp0"

setlocal
call SetVersion.cmd
set DISTDIR=.\Build\Releases
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%

rmdir /q /s "%DISTDIR%\zip-version" > NUL 2> NUL
rmdir /q /s "%DISTDIR%\x64-zip-version" > NUL 2> NUL
mkdir "%DISTDIR%" 2> NUL

copy "Build\WinMerge-%RCVER%-Setup.exe" "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe"
copy "Build\WinMerge-%RCVER%-x64-Setup.exe" "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe"

for %%i in (Languages Filters MergePlugins Docs Frhed\Docs Frhed\Languages WinIMerge Merge7z\Lang GnuWin32) do (
  mkdir "%DISTDIR%\zip-version\WinMerge\%%i" 2> NUL
  mkdir "%DISTDIR%\x64-zip-version\WinMerge\%%i" 2> NUL
)

rem Docs
for %%i in (Translations\Docs\Readme\ReadMe-*.txt Build\Manual\htmlhelp\WinMerge.chm Docs\Users\ReleaseNotes.html Docs\Users\ChangeLog.txt) do (
  copy "%%i" "%DISTDIR%\zip-version\WinMerge\Docs"
  copy "%%i" "%DISTDIR%\x64-zip-version\WinMerge\Docs"
)
for %%i in (Src\COPYING Docs\Users\Contributors.txt Docs\Users\Files.txt Docs\Users\ReadMe.txt) do (
  copy "%%i" "%DISTDIR%\zip-version\WinMerge"
  copy "%%i" "%DISTDIR%\x64-zip-version\WinMerge"
)

rem Excecutables
copy Build\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\WinMergeU.exe "%DISTDIR%\x64-zip-version\WinMerge\"
copy Plugins\WinMerge32BitPluginProxy\Release\WinMerge32BitPluginProxy.exe "%DISTDIR%\x64-zip-version\WinMerge\"

rem ShellExtension
copy "Build\ShellExtension\ShellExtensionU.dll" "%DISTDIR%\zip-version\WinMerge\"
copy "Build\ShellExtension\ShellExtensionX64.dll" "%DISTDIR%\zip-version\WinMerge\"
copy "Build\ShellExtension\ShellExtensionX64.dll" "%DISTDIR%\x64-zip-version\WinMerge\"
copy ShellExtension\*Register.bat "%DISTDIR%\zip-version\WinMerge\"
copy ShellExtension\*Register.bat "%DISTDIR%\x64-zip-version\WinMerge\"

rem Translation
copy Build\MergeUnicodeRelease\Languages\*.po "%DISTDIR%\zip-version\WinMerge\Languages\"
copy Build\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\zip-version\WinMerge\"
copy Build\X64\MergeUnicodeRelease\Languages\*.po "%DISTDIR%\x64-zip-version\WinMerge\Languages\"
copy Build\X64\MergeUnicodeRelease\MergeLang.dll "%DISTDIR%\x64-zip-version\WinMerge\"

rem Filters
for %%i in (Filters\*.flt Filters\*.tmpl Filters\*.txt) do (
  copy "%%i" "%DISTDIR%\zip-version\WinMerge\Filters"
  copy "%%i" "%DISTDIR%\x64-zip-version\WinMerge\Filters"
)

rem Plugins
copy Plugins\dlls\*.dll "%DISTDIR%\zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\X64\*.dll "%DISTDIR%\x64-zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\x64-zip-version\WinMerge\MergePlugins\"
del "%DISTDIR%\zip-version\WinMerge\MergePlugins\CompareMS*.dll"
del "%DISTDIR%\zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"
del "%DISTDIR%\zip-version\WinMerge\MergePlugins\DisplayXMLFiles.dll"
del "%DISTDIR%\x64-zip-version\WinMerge\MergePlugins\Watch*OfLog.dll"
del "%DISTDIR%\x64-zip-version\WinMerge\MergePlugins\DisplayXMLFiles.dll"

rem 7zPlugins
copy Build\Merge7z\Merge7z*.dll "%DISTDIR%\zip-version\WinMerge\Merge7z\"
copy Build\Merge7z\7z.dll "%DISTDIR%\zip-version\WinMerge\Merge7z\"
copy Build\Merge7z\*.txt "%DISTDIR%\zip-version\WinMerge\Merge7z\"
copy Build\Merge7z\Lang\*.txt "%DISTDIR%\zip-version\WinMerge\Merge7z\Lang\"
copy Build\X64\Merge7z\Merge7z*.dll "%DISTDIR%\x64-zip-version\WinMerge\Merge7z\"
copy Build\X64\Merge7z\7z.dll "%DISTDIR%\x64-zip-version\WinMerge\Merge7z\"
copy Build\X64\Merge7z\*.txt "%DISTDIR%\x64-zip-version\WinMerge\Merge7z\"
copy Build\X64\Merge7z\Lang\*.txt "%DISTDIR%\x64-zip-version\WinMerge\Merge7z\Lang\"

rem Frhed
copy Externals\Frhed\GPL.txt "%DISTDIR%\zip-version\WinMerge\frhed\"
rem copy Build\Frhed\frhed.exe "%DISTDIR%\zip-version\WinMerge\frhed\"
copy Build\Frhed\hekseditU.dll "%DISTDIR%\zip-version\WinMerge\frhed\"
copy Build\Frhed\Docs\*.txt "%DISTDIR%\zip-version\WinMerge\frhed\Docs"
copy Build\Frhed\Docs\Sample.tpl "%DISTDIR%\zip-version\WinMerge\frhed\Docs"
copy Build\Frhed\Languages\*.po "%DISTDIR%\zip-version\WinMerge\frhed\Languages"
copy Build\Frhed\Languages\heksedit.lng "%DISTDIR%\zip-version\WinMerge\frhed\Languages"
copy Build\Frhed\GPL.txt "%DISTDIR%\x64-zip-version\WinMerge\frhed\"
rem copy Build\x64\Frhed\frhed.exe "%DISTDIR%\x64-zip-version\WinMerge\frhed\"
copy Build\x64\Frhed\hekseditU.dll "%DISTDIR%\x64-zip-version\WinMerge\frhed\"
copy Build\x64\Frhed\Docs\*.txt "%DISTDIR%\x64-zip-version\WinMerge\frhed\Docs"
copy Build\x64\Frhed\Docs\Sample.tpl "%DISTDIR%\x64-zip-version\WinMerge\frhed\Docs"
copy Build\x64\Frhed\Languages\*.po "%DISTDIR%\x64-zip-version\WinMerge\frhed\Languages"
copy Build\x64\Frhed\Languages\heksedit.lng "%DISTDIR%\x64-zip-version\WinMerge\frhed\Languages"

rem WinIMerge
copy Build\WinIMerge\GPL.txt "%DISTDIR%\zip-version\WinMerge\WinIMerge\"
copy Build\WinIMerge\freeimage-license-gplv2.txt "%DISTDIR%\zip-version\WinMerge\WinIMerge\"
rem copy Build\WinIMerge\bin\WinIMerge.exe "%DISTDIR%\zip-version\WinMerge\WinIMerge\"
copy Build\WinIMerge\bin\WinIMergeLib.dll "%DISTDIR%\zip-version\WinMerge\WinIMerge\"
copy Build\WinIMerge\bin\vcomp*.dll "%DISTDIR%\zip-version\WinMerge\"
copy Build\WinIMerge\GPL.txt "%DISTDIR%\x64-zip-version\WinMerge\WinIMerge\"
copy Build\WinIMerge\freeimage-license-gplv2.txt "%DISTDIR%\x64-zip-version\WinMerge\WinIMerge\"
rem copy Build\WinIMerge\bin64\WinIMerge.exe "%DISTDIR%\x64-zip-version\WinMerge\WinIMerge\"
copy Build\WinIMerge\bin64\WinIMergeLib.dll "%DISTDIR%\x64-zip-version\WinMerge\WinIMerge\"
copy Build\WinIMerge\bin64\vcomp*.dll "%DISTDIR%\x64-zip-version\WinMerge\"

rem Patch
xcopy /s/y Build\GnuWin32 "%DISTDIR%\zip-version\WinMerge\GnuWin32\"
xcopy /s/y Build\GnuWin32 "%DISTDIR%\x64-zip-version\WinMerge\GnuWin32\"

7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip" "%DISTDIR%\zip-version\WinMerge\"
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip" "%DISTDIR%\x64-zip-version\WinMerge\"

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"

7z.exe a -t7z -xr!*.gcno -xr!*.gcda -xr!*.gcov -xr!*.orig -xr!*.o -xr!*.a -xr!.dep -xr!*.out -xr!debug_static* -xr!debug_shared* -xr!release_static* -xr!release_shared* -xr!*.bak -xr!*.lang -xr!*.db -xr!*.ncb  -xr!*.sdf -xr!*.bsc -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.ilk -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!.vs -xr!*.VC.db-* -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.exp -xr!*.idb -xr!*.%COMPUTERNAME%.%USERNAME%.user -xr!BuildLog.htm -xr!ipch -xr!*.tlb -xr!*.tlog -xr!*.*~ -xr!CVS -xr!.svn -xr!.hg -xr!*.log -xr!*.lastbuildstate -xr!FreeImage*.dll -xr!WinIMergeLib.dll -xr!WinIMerge.exe -xr!BuildTmp -xr!Docs\Users\Manual\build\dtd -xr!Docs\Users\Manual\build\hhc -xr!Docs\Users\Manual\build\saxon -xr!Docs\Users\Manual\build\xerces -xr!Docs\Users\Manual\build\xsl "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Translations Web readme.txt Version.h *.cmd *.bat *.inf *.sln *.vbs

(
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"
) > "%DISTDIR%\files.txt"

popd
