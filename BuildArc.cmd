pushd "%~dp0"

setlocal
call SetVersion.cmd
if "%2" == "-ci" (
  if exist .hg (
    for /F "delims=" %%i in ('hg id') do set SAFEAPPVER=%SAFEAPPVER%-%%i
  ) else if exist .git (
    for /F "delims=" %%i in ('git rev-parse --short head') do set SAFEAPPVER=%SAFEAPPVER%-%%i
  ) else (
    set SAFEAPPVER=%SAFEAPPVER%-%APPVEYOR_BUILD_VERSION%
  )
)
set DISTDIR=.\Build\Releases
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%

if "%1" == "" (
  call :BuildZip 
  call :BuildZip x64
  call :BuildZip ARM64
) else (
  call :BuildZip %1 
)

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" 2> NUL

7z.exe a -t7z -xr!*.gcno -xr!*.gcda -xr!*.gcov -xr!*.orig -xr!*.o -xr!*.a -xr!.dep -xr!*.asm -xr!*.out -xr!debug_static* -xr!debug_shared* -xr!release_static* -xr!release_shared* -xr!*.bak -xr!*.lang -xr!*.db -xr!*.ncb  -xr!*.sdf -xr!*.bsc -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.ilk -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!.vs -xr!*.VC.db-* -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.exp -xr!*.idb -xr!*.%COMPUTERNAME%.%USERNAME%.user -xr!BuildLog.htm -xr!ipch -xr!*.tlb -xr!*.tlog -xr!*.*~ -xr!CVS -xr!.svn -xr!.hg -xr!*.log -xr!*.lastbuildstate -xr!FreeImage*.dll -xr!WinIMergeLib.dll -xr!WinIMerge.exe -xr!BuildTmp -xr!Docs\Manual\Tools "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport ColorSchemes Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Translations Web readme.txt Version.h Version.in *.cmd *.bat *.inf *.sln *.vbs

(
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-PerUser-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-ARM64-Setup.exe"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-ARM64-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-pdb.7z"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-pdb.7z"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-ARM64-pdb.7z"
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

echo.
echo ============================================================
echo BUILD winmerge-%SAFEAPPVER%-%PLATFORMH%exe.zip...
echo ============================================================

rmdir /q /s "%DISTDIR%\%PLATFORMH%zip-version" > NUL 2> NUL
mkdir "%DISTDIR%" 2> NUL

rem Copy platform setups
copy "Build\WinMerge-%RCVER%-%PLATFORMH%Setup.exe" "%DISTDIR%\WinMerge-%SAFEAPPVER%-%PLATFORMH%Setup.exe" > NUL
if not "%1" == "" (
  copy "Build\WinMerge-%RCVER%-%PLATFORMH%PerUser-Setup.exe" "%DISTDIR%\WinMerge-%SAFEAPPVER%-%PLATFORMH%PerUser-Setup.exe" > NUL
)

rem Create folder structure
for %%i in (ColorSchemes Languages Filters MergePlugins Docs Frhed\Docs Frhed\Languages WinIMerge Merge7z\Lang Commands\Apache-Tika Commands\tidy-html5 Commands\jq Commands\q Commands\GnuWin32) do (
  mkdir "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\%%i" 2> NUL
)

rem Docs
echo Copy Docs...
for %%i in (Translations\Docs\Readme\ReadMe-*.txt Build\Manual\htmlhelp\WinMerge*.chm Docs\Users\ReleaseNotes.html Docs\Users\ChangeLog.html) do (
  copy "%%i" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Docs" > NUL
)
for %%i in (Src\COPYING Docs\Users\Contributors.txt Docs\Users\ReadMe.txt) do (
  copy "%%i" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge" > NUL
)

rem Excecutables
echo Copy Excecutables...
copy Build\%PLATFORM%\Release\WinMergeU.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
if not "%1" == "" (
  copy Plugins\WinMerge32BitPluginProxy\Release\WinMerge32BitPluginProxy.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)

rem ShellExtension
echo Copy ShellExtension...
if not "%1" == "ARM64" (
  copy "Build\ShellExtension\ShellExtensionU.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\ShellExtensionX64.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)
if "%1" == "ARM64" (
  copy "Build\ShellExtension\ShellExtensionARM64.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)
copy ShellExtension\*Register*.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL

rem Translations
echo Copy Translations...
copy Build\%PLATFORM%\Release\Languages\*.po "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Languages\" > NUL

rem ColorSchemes
echo Copy ColorSchemes...
for %%i in (ColorSchemes\*.ini) do (
  copy "%%i" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\ColorSchemes" > NUL
)

rem Filters
echo Copy Filters...
for %%i in (Filters\*.flt Filters\*.tmpl Filters\*.txt) do (
  copy "%%i" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Filters" > NUL
)

rem Plugins
echo Copy Plugins...
copy Plugins\dlls\%PLATFORM%\*.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\" > NUL
copy Plugins\dlls\*.sct "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\" > NUL
del "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\CompareMS*.dll" 2> NUL
del "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\Watch*OfLog.dll" 2> NUL
del "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\DisplayXMLFiles.dll" 2> NUL

rem 7zPlugins
echo Copy 7zPlugins...
copy Build\%PLATFORM%\Merge7z\Merge7z.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\" > NUL
copy Build\%PLATFORM%\Merge7z\7z.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\" > NUL
copy Build\%PLATFORM%\Merge7z\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\" > NUL
copy Build\%PLATFORM%\Merge7z\Lang\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\Lang\" > NUL

rem Frhed
echo Copy Frhed...
copy Build\%PLATFORM%\Frhed\GPL.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\" > NUL
rem copy Build\%PLATFORM%\Frhed\frhed.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\" > NUL
copy Build\%PLATFORM%\Frhed\hekseditU.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\" > NUL
copy Build\%PLATFORM%\Frhed\Docs\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Docs" > NUL
copy Build\%PLATFORM%\Frhed\Docs\Sample.tpl "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Docs" > NUL
copy Build\%PLATFORM%\Frhed\Languages\*.po "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Languages" > NUL
copy Build\%PLATFORM%\Frhed\Languages\heksedit.lng "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Languages" > NUL

rem WinIMerge
echo Copy WinIMerge...
copy Build\WinIMerge\GPL.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\" > NUL
copy Build\WinIMerge\freeimage-license-gplv2.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\" > NUL
if "%1" == "" (
  rem copy Build\WinIMerge\bin\WinIMerge.exe "%DISTDIR%\zip-version\WinMerge\WinIMerge\" > NUL
  copy Build\WinIMerge\bin\WinIMergeLib.dll "%DISTDIR%\zip-version\WinMerge\WinIMerge\" > NUL
  copy Build\WinIMerge\bin\vcomp*.dll "%DISTDIR%\zip-version\WinMerge\" > NUL
) else (
  rem copy Build\WinIMerge\bin64\WinIMerge.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\" > NUL
  copy Build\WinIMerge\bin64\WinIMergeLib.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\" > NUL
  copy Build\WinIMerge\bin64\vcomp*.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)

rem Commands
echo Copy Commands...
xcopy /s/y Plugins\Commands "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands"

rem Patch
echo Copy Patch...
xcopy /s/y Build\GnuWin32 "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\GnuWin32\" > NUL

rem Copy jq...
echo Copy jq...
copy Build\jq\jq-win32.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\jq\jq.exe" > NUL
copy Build\jq\jq-jq-1.4\COPYING "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\jq\" > NUL

rem Copy tidy-html5...
echo Copy tidy-html5...
copy Build\tidy-html5\bin\tidy.* "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\tidy-html5\" > NUL
copy Build\tidy-html5\tidy-html5-5.4.0\README\LICENSE.md "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\tidy-html5\" > NUL

rem Plugin.xml
copy Plugins\Plugins.xml "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\" > NUL

echo.
echo ------------------------------------------------------------
echo Pack archive...
echo ------------------------------------------------------------
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-%PLATFORMH%exe.zip" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"
set PDBFILE="Build\%PLATFORM%\Release\WinMergeU.pdb"
7z.exe a -t7z  "%DISTDIR%\winmerge-%SAFEAPPVER%-%PLATFORMH%pdb.7z" %PDBFILE:\\=\%

goto :eof
