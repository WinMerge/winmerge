@echo off
setlocal
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%
7z > NUL
if not %ERRORLEVEL% == 0 (
  echo 7-Zip is not installed
  goto :eof
)
@echo on

set downloadsdir=%~dp0\build\WinMergeDownloadDeps
set urls_destdirs=^
https://github.com/WinMerge/winmerge/releases/download/winmerge_manual_another_build_tools_v2/winmerge_manual_another_build_tools_v2.zip!Docs\Manual\Tools ^
https://github.com/WinMerge/winmerge/releases/download/ShellExtension-1.18.7.0/ShellExtension-1.18.7.0.zip!Build ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2201.0/Merge7z2201.0-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2201.0/Merge7z2201.0-x64.zip!Build\X64\Release ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2201.0/Merge7z2201.0-ARM.zip!Build\ARM64\Release ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2201.0/Merge7z2201.0-ARM64.zip!Build\ARM64\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.12-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.12-x64.zip!Build\x64\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.12-ARM.zip!Build\ARM\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.12-ARM64.zip!Build\ARM64\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.39/winimerge-1.0.39.0-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.39/winimerge-1.0.39.0-x64.zip!Build\x64\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.39/winimerge-1.0.39.0-ARM.zip!Build\ARM\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.39/winimerge-1.0.39.0-ARM64.zip!Build\ARM64\Release ^
https://github.com/WinMerge/winwebdiff/releases/download/v1.0.6/winwebdiff-1.0.6.0-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/winwebdiff/releases/download/v1.0.6/winwebdiff-1.0.6.0-x64.zip!Build\x64\Release ^
https://github.com/WinMerge/winwebdiff/releases/download/v1.0.6/winwebdiff-1.0.6.0-ARM64.zip!Build\ARM64\Release ^
https://github.com/WinMerge/patch/releases/download/v2.5.9-7/patch-2.5.9-7-bin.zip!Build\GnuWin32 ^
https://github.com/htacg/tidy-html5/releases/download/5.4.0/tidy-5.4.0-w32-mt-XP.zip!Build\tidy-html5 ^
https://github.com/htacg/tidy-html5/archive/refs/tags/5.4.0.zip!Build\tidy-html5 ^
https://github.com/stedolan/jq/releases/download/jq-1.4/jq-win32.exe!Build\jq ^
https://github.com/stedolan/jq/archive/refs/tags/jq-1.4.zip!Build\jq ^
https://github.com/facebook/zstd/releases/download/v1.5.2/zstd-v1.5.2-win64.zip!Build\zstd ^
https://mirror.msys2.org/mingw/mingw32/mingw-w64-i686-md4c-0.4.8-2-any.pkg.tar.zst!Build\md4c ^
http://www.magicnotes.com/steelbytes/SBAppLocale_ENG.zip!Docs\Manual\Tools

pushd "%~dp0"
mkdir %downloadsdir% 2> NUL

for %%p in (%urls_destdirs%) do (
  for /F "tokens=1,2 delims=!" %%u in ("%%p") do (
    if not exist %downloadsdir%\%%~nxu (
      powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %%u -Outfile %downloadsdir%\%%~nxu"
    )
    if "%%~xu" == ".zip" (
      7z x %downloadsdir%\%%~nxu -aoa -o%%v
    ) else (
      mkdir %%v > NUL
      if "%%~xu" == ".zst" (
        Build\zstd\zstd-v1.5.2-win64\zstd.exe -dc %downloadsdir%\%%~nxu | tar xf - -C %%v
      ) else (
        copy %downloadsdir%\%%~nxu %%v
      )
    )
  )
)

for /d %%i in (build\tidy-html5\tidy-5.4.0-w32-mt-XP\*) do move %%i build\tidy-html5\

for %%i in (x86 x64 ARM ARM64) do (
  for %%j in (Release Debug Test) do (
    mkdir Build\%%i\%%j\Merge7z 2> NUL
    mkdir Build\%%i\%%j\WinIMerge 2> NUL
    mkdir Build\%%i\%%j\WinWebDiff 2> NUL
    mkdir Build\%%i\%%j\Frhed 2> NUL
    mkdir Build\%%i\%%j\Filters 2> NUL
    mkdir Build\%%i\%%j\ColorSchemes 2> NUL
    mkdir Build\%%i\%%j\MergePlugins 2> NUL
    mkdir Build\%%i\%%j\Commands\jq 2> NUL
    mkdir Build\%%i\%%j\Commands\tidy-html5 2> NUL
    mkdir Build\%%i\%%j\Commands\GnuWin32 2> NUL
    mkdir Build\%%i\%%j\Commands\md4c 2> NUL
    xcopy /s/y Build\%%i\Release\Merge7z Build\%%i\%%j\Merge7z\
    xcopy /s/y Build\%%i\Release\Frhed Build\%%i\%%j\Frhed\
    copy Build\%%i\Release\WinIMerge\WinIMergeLib.dll Build\%%i\%%j\WinIMerge\
    copy Build\%%i\Release\WinWebDiff\WinWebDiffLib.dll Build\%%i\%%j\WinWebDiff\ 2> NUL
    xcopy /s/y Build\GnuWin32 Build\%%i\%%j\Commands\GnuWin32\
    copy Build\jq\jq-win32.exe Build\%%i\%%j\Commands\jq\jq.exe
    copy Build\jq\jq-jq-1.4\COPYING Build\%%i\%%j\Commands\jq\
    copy Build\tidy-html5\bin\tidy.* Build\%%i\%%j\Commands\tidy-html5\
    copy Build\tidy-html5\tidy-html5-5.4.0\README\LICENSE.md Build\%%i\%%j\Commands\tidy-html5\
    copy Build\md4c\mingw32\bin\*.exe Build\%%i\%%j\Commands\md4c\
    copy Build\md4c\mingw32\bin\*.dll Build\%%i\%%j\Commands\md4c\
    copy Build\md4c\mingw32\share\licenses\md4c\LICENSE.md Build\%%i\%%j\Commands\md4c\
    xcopy /s/y Plugins\Commands Build\%%i\%%j\Commands
    xcopy /s/y Filters Build\%%i\%%j\Filters\
    xcopy /s/y ColorSchemes Build\%%i\%%j\ColorSchemes\
    xcopy /s/y Plugins\dlls\*.sct Build\%%i\%%j\MergePlugins\
    xcopy /s/y Plugins\Plugins.xml Build\%%i\%%j\MergePlugins\
    xcopy /s/y Build\ShellExtension\WinMergeContextMenuPackage.msix Build\%%i\%%j
    copy Plugins\dlls\%%i\*.dll Build\%%i\%%j\MergePlugins\
  )
)

popd
