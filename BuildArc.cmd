pushd "%~dp0"

setlocal
call SetVersion.cmd
if "%2" == "-ci" (
  if exist .hg (
    for /F "delims=" %%i in ('hg id') do set SAFEAPPVER=%SAFEAPPVER%-%DATE:/=-%-%%i
  ) else if exist .git (
    for /F "delims=" %%i in ('git rev-parse --short head') do set SAFEAPPVER=%SAFEAPPVER%-%DATE:/=-%-%%i
  ) else (
    set SAFEAPPVER=%SAFEAPPVER%-%DATE:/=-%-%APPVEYOR_BUILD_VERSION%
  )
)
set DISTDIR=.\Build\Releases
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%

if "%1" == "" (
  call :BuildZip 
  call :BuildZip x64
) else (
  call :BuildZip %1 
)

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" 2> NUL

7z.exe a -t7z -xr!*.gcno -xr!*.gcda -xr!*.gcov -xr!*.orig -xr!*.o -xr!*.a -xr!.dep -xr!*.out -xr!debug_static* -xr!debug_shared* -xr!release_static* -xr!release_shared* -xr!*.bak -xr!*.lang -xr!*.db -xr!*.ncb  -xr!*.sdf -xr!*.bsc -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.ilk -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!.vs -xr!*.VC.db-* -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.exp -xr!*.idb -xr!*.%COMPUTERNAME%.%USERNAME%.user -xr!BuildLog.htm -xr!ipch -xr!*.tlb -xr!*.tlog -xr!*.*~ -xr!CVS -xr!.svn -xr!.hg -xr!*.log -xr!*.lastbuildstate -xr!FreeImage*.dll -xr!WinIMergeLib.dll -xr!WinIMerge.exe -xr!BuildTmp -xr!Docs\Users\Manual\build\dtd -xr!Docs\Users\Manual\build\hhc -xr!Docs\Users\Manual\build\saxon -xr!Docs\Users\Manual\build\xerces -xr!Docs\Users\Manual\build\xsl "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Translations Web readme.txt Version.h *.cmd *.bat *.inf *.sln *.vbs

(
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-%PLATFORMH%Setup.exe"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-%PLATFORMH%exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"
) > "%DISTDIR%\files.txt"

popd
goto :eof


:BuildZip

set PLATFORM=%1
if "%1" == "" (
  set PLATFORMH=%1
) else (
  set PLATFORMH=%1-
)

rmdir /q /s "%DISTDIR%\%PLATFORMH%zip-version" > NUL 2> NUL
mkdir "%DISTDIR%" 2> NUL

copy "Build\WinMerge-%RCVER%-%PLATFORMH%Setup.exe" "%DISTDIR%\WinMerge-%SAFEAPPVER%-%PLATFORMH%Setup.exe"

for %%i in (Languages Filters MergePlugins Docs Frhed\Docs Frhed\Languages WinIMerge Merge7z\Lang GnuWin32) do (
  mkdir "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\%%i" 2> NUL
)

rem Docs
for %%i in (Translations\Docs\Readme\ReadMe-*.txt Build\Manual\htmlhelp\WinMerge.chm Docs\Users\ReleaseNotes.html Docs\Users\ChangeLog.txt) do (
  copy "%%i" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Docs"
)
for %%i in (Src\COPYING Docs\Users\Contributors.txt Docs\Users\ReadMe.txt) do (
  copy "%%i" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge"
)

rem Excecutables
copy Build\%PLATFORM%\Release\WinMergeU.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"
if not "%1" == "" (
  copy Plugins\WinMerge32BitPluginProxy\Release\WinMerge32BitPluginProxy.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"
)

rem ShellExtension
copy "Build\ShellExtension\ShellExtensionU.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"
copy "Build\ShellExtension\ShellExtensionX64.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"
copy ShellExtension\*Register*.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"

rem Translation
copy Build\%PLATFORM%\Release\Languages\*.po "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Languages\"

rem Filters
for %%i in (Filters\*.flt Filters\*.tmpl Filters\*.txt) do (
  copy "%%i" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Filters"
)

rem Plugins
copy Plugins\dlls\%PLATFORM%\*.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\"
copy Plugins\dlls\*.sct "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\"
del "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\CompareMS*.dll" 2> NUL
del "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\Watch*OfLog.dll" 2> NUL
del "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\DisplayXMLFiles.dll" 2> NUL

rem 7zPlugins
copy Build\%PLATFORM%\Merge7z\Merge7z.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\"
copy Build\%PLATFORM%\Merge7z\7z.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\"
copy Build\%PLATFORM%\Merge7z\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\"
copy Build\%PLATFORM%\Merge7z\Lang\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\Lang\"

rem Frhed
copy Build\%PLATFORM%\Frhed\GPL.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\"
rem copy Build\%PLATFORM%\Frhed\frhed.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\"
copy Build\%PLATFORM%\Frhed\hekseditU.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\"
copy Build\%PLATFORM%\Frhed\Docs\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Docs"
copy Build\%PLATFORM%\Frhed\Docs\Sample.tpl "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Docs"
copy Build\%PLATFORM%\Frhed\Languages\*.po "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Languages"
copy Build\%PLATFORM%\Frhed\Languages\heksedit.lng "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Languages"

rem WinIMerge
copy Build\WinIMerge\GPL.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\"
copy Build\WinIMerge\freeimage-license-gplv2.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\"
if "%1" == "" (
  rem copy Build\WinIMerge\bin\WinIMerge.exe "%DISTDIR%\zip-version\WinMerge\WinIMerge\"
  copy Build\WinIMerge\bin\WinIMergeLib.dll "%DISTDIR%\zip-version\WinMerge\WinIMerge\"
  copy Build\WinIMerge\bin\vcomp*.dll "%DISTDIR%\zip-version\WinMerge\"
) else (
  rem copy Build\WinIMerge\bin64\WinIMerge.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\"
  copy Build\WinIMerge\bin64\WinIMergeLib.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\"
  copy Build\WinIMerge\bin64\vcomp*.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"
)

rem Patch
xcopy /s/y Build\GnuWin32 "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\GnuWin32\"

7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-%PLATFORMH%exe.zip" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"

goto :eof
