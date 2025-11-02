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
  call :BuildZip x86
  call :BuildZip x64
  call :BuildZip ARM
  call :BuildZip ARM64
) else (
  call :BuildZip %1 
)

del "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" 2> NUL

7z.exe a -t7z -xr!*.gcno -xr!*.gcda -xr!*.gcov -xr!*.orig -xr!*.o -xr!*.a -xr!.dep -xr!*.asm -xr!*.out -xr!debug_static* -xr!debug_shared* -xr!release_static* -xr!release_shared* -xr!*.bak -xr!*.lang -xr!*.db -xr!*.ncb  -xr!*.sdf -xr!*.bsc -xr!*.opt -xr!*.plg -xr!*.suo -xr!*.obj -xr!*.ilk -xr!*.pdb -xr!*.pch -xr!*.res -xr!*.exe -xr!*.sbr -xr!.vs -xr!*.VC.db-* -xr!*.zip -xr!WinMergeSplash.psd -xr!*.lib -xr!*.exp -xr!*.idb -xr!*.%COMPUTERNAME%.%USERNAME%.user -xr!BuildLog.htm -xr!ipch -xr!*.tlb -xr!*.tlog -xr!*.*~ -xr!CVS -xr!.svn -xr!.hg -xr!*.log -xr!*.lastbuildstate -xr!FreeImage*.dll -xr!WinIMergeLib.dll -xr!WinIMerge.exe -xr!WinWebDiff.dll -xr!WinWebDiff.exe -xr!darkmode.dll -xr!BuildTmp -xr!Docs\Manual\Tools "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z" ArchiveSupport ColorSchemes Docs Externals Filters Installer Plugins ShellExtension Src Testing Tools Translations Web readme.txt Version.h Version.in *.cmd *.bat *.inf *.sln *.js

(
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-x64-PerUser-Setup.exe"
echo "%DISTDIR%\WinMerge-%SAFEAPPVER%-ARM64-Setup.exe"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-ARM-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-ARM64-exe.zip"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-pdb.7z"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-x64-pdb.7z"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-ARM-pdb.7z"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-ARM64-pdb.7z"
echo "%DISTDIR%\winmerge-%SAFEAPPVER%-full-src.7z"
) > "%DISTDIR%\files.txt"

popd
goto :eof


:BuildZip

set PLATFORM=%1
if "%1" == "x86" (
  set PLATFORMH=
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
if not "%1" == "ARM" (
  copy "Build\WinMerge-%RCVER%-%PLATFORMH%Setup.exe" "%DISTDIR%\WinMerge-%SAFEAPPVER%-%PLATFORMH%Setup.exe" > NUL
)
if "%1" == "x64" (
  copy "Build\WinMerge-%RCVER%-%PLATFORMH%PerUser-Setup.exe" "%DISTDIR%\WinMerge-%SAFEAPPVER%-%PLATFORMH%PerUser-Setup.exe" > NUL
)

rem Create folder structure
for %%i in (ColorSchemes Languages\ShellExtension Filters MergePlugins Docs Frhed\Docs Frhed\Languages WinIMerge WinWebDiff darkmodelib Merge7z\Lang Commands\Apache-Tika Commands\tidy-html5 Commands\jq Commands\q Commands\msys2 Commands\md4c Resources) do (
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
if "%1" == "x64" (
  copy Plugins\WinMerge32BitPluginProxy\Release\WinMerge32BitPluginProxy.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)

rem ShellExtension
echo Copy ShellExtension...
if "%1" == "x86" (
  copy "Build\ShellExtension\ShellExtensionU.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\ShellExtensionX64.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\x64\WinMergeContextMenu.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)
if "%1" == "x64" (
  copy "Build\ShellExtension\ShellExtensionU.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\ShellExtensionX64.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\x64\WinMergeContextMenu.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)
if "%1" == "ARM" (
  copy "Build\ShellExtension\ShellExtensionARM.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\ShellExtensionARM64.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\ARM64\WinMergeContextMenu.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)
if "%1" == "ARM64" (
  copy "Build\ShellExtension\ShellExtensionARM64.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
  copy "Build\ShellExtension\ARM64\WinMergeContextMenu.dll" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
)
copy ShellExtension\Register.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
copy ShellExtension\RegisterPerUser.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
copy ShellExtension\RegisterWinMergeContextMenuPackage.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
copy ShellExtension\UnRegister.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
copy ShellExtension\UnRegisterPerUser.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
copy ShellExtension\UnregisterWinMergeContextMenuPackage.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL
copy "Build\ShellExtension\WinMergeContextMenuPackage.msix" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL

rem Translations
echo Copy Translations...
copy Build\%PLATFORM%\Release\Languages\*.po "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Languages\" > NUL
copy Translations\ShellExtension\*.po "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Languages\ShellExtension" > NUL

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
copy Build\%PLATFORM%\Release\Merge7z\Merge7z.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\" > NUL
copy Build\%PLATFORM%\Release\Merge7z\7z.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\" > NUL
copy Build\%PLATFORM%\Release\Merge7z\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\" > NUL
copy Build\%PLATFORM%\Release\Merge7z\Lang\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Merge7z\Lang\" > NUL

rem Frhed
echo Copy Frhed...
copy Build\%PLATFORM%\Release\Frhed\GPL.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\" > NUL
copy Build\%PLATFORM%\Release\Frhed\hekseditU.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\" > NUL
copy Build\%PLATFORM%\Release\Frhed\Docs\*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Docs" > NUL
copy Build\%PLATFORM%\Release\Frhed\Docs\Sample.tpl "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Docs" > NUL
copy Build\%PLATFORM%\Release\Frhed\Languages\*.po "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Languages" > NUL
copy Build\%PLATFORM%\Release\Frhed\Languages\heksedit.lng "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\frhed\Languages" > NUL

rem WinIMerge
echo Copy WinIMerge...
copy Build\%PLATFORM%\Release\WinIMerge\GPL.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\" > NUL
copy Build\%PLATFORM%\Release\WinIMerge\freeimage-license-gplv2.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\" > NUL
copy Build\%PLATFORM%\Release\WinIMerge\WinIMergeLib.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinIMerge\" > NUL
copy Build\%PLATFORM%\Release\WinIMerge\vcomp*.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\" > NUL

rem WinWebDiff
echo Copy WinWebDiff...
copy Build\%PLATFORM%\Release\WinWebDiff\LICENSE*.txt "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinWebDiff\" > NUL
copy Build\%PLATFORM%\Release\WinWebDiff\WinWebDiffLib.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\WinWebDiff\" > NUL

rem darkmodelib
echo Copy darkmodelib...
copy Build\%PLATFORM%\Release\darkmodelib\LICENSE.md "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\darkmodelib\" > NUL
copy Build\%PLATFORM%\Release\darkmodelib\darkmode.dll "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\darkmodelib\" > NUL

rem Commands
echo Copy Commands...
xcopy /s/y Plugins\Commands "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands"
copy Plugins\Commands\DownloadFiles.bat "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands"

rem Patch
echo Copy Patch...
xcopy /s/y Build\msys2 "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\msys2\" > NUL

rem Copy jq...
echo Copy jq...
copy Build\jq\jq-windows-i386.exe "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\jq\jq.exe" > NUL
copy Build\jq\jq-jq-1.7.1\COPYING "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\jq\" > NUL

rem Copy tidy-html5...
echo Copy tidy-html5...
copy Build\tidy-html5\bin\tidy.* "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\tidy-html5\" > NUL
copy Build\tidy-html5\tidy-html5-5.4.0\README\LICENSE.md "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\tidy-html5\" > NUL

rem Copy md4c...
echo Copy md4c...
copy Build\md4c\mingw32\bin\*.* "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\md4c\" > NUL
copy Build\md4c\mingw32\share\licenses\md4c\LICENSE.md "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Commands\md4c\" > NUL

rem Plugin.xml
copy Plugins\Plugins.xml "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\MergePlugins\" > NUL

rem Copy Resources...
echo Copy Resources...
copy Src\res\splash.png "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\Resources\" > NUL

echo.
echo ------------------------------------------------------------
echo Pack archive...
echo ------------------------------------------------------------
7z.exe a -tzip "%DISTDIR%\winmerge-%SAFEAPPVER%-%PLATFORMH%exe.zip" "%DISTDIR%\%PLATFORMH%zip-version\WinMerge\"
set PDBFILE="Build\%PLATFORM%\Release\WinMergeU.pdb"
7z.exe a -t7z  "%DISTDIR%\winmerge-%SAFEAPPVER%-%PLATFORMH%pdb.7z" %PDBFILE:\\=\%

goto :eof
