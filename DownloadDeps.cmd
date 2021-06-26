setlocal
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%
set downloadsdir=%~dp0\build\WinMergeDownloadDeps
set urls_destdirs=^
https://github.com/WinMerge/winmerge/releases/download/winmerge_manual_another_build_tools_v2/winmerge_manual_another_build_tools_v2.zip!Docs\Manual\Tools ^
https://github.com/WinMerge/winmerge/releases/download/ShellExtension-1.17.14.0/ShellExtension-1.17.14.0.zip!Build ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z1900.5/Merge7z1900.5-win32.zip!Build ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z1900.5/Merge7z1900.5-x64.zip!Build\X64 ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z1900.5/Merge7z1900.5-ARM64.zip!Build\ARM64 ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.7-win32.zip!Build ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.7-x64.zip!Build\X64 ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.7-ARM64.zip!Build\ARM64 ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.29/winimerge-1.0.29.0-exe.zip!Build ^
https://github.com/WinMerge/patch/releases/download/v2.5.9-7/patch-2.5.9-7-bin.zip!Build\GnuWin32 ^
https://github.com/htacg/tidy-html5/releases/download/5.4.0/tidy-5.4.0-w32-mt-XP.zip!Build\tidy-html5 ^
https://github.com/htacg/tidy-html5/archive/refs/tags/5.4.0.zip!Build\tidy-html5 ^
https://github.com/stedolan/jq/releases/download/jq-1.4/jq-win32.exe!Build\jq ^
https://github.com/stedolan/jq/archive/refs/tags/jq-1.4.zip!Build\jq ^
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
      copy %downloadsdir%\%%~nxu %%v
    )
  )
)

for /d %%i in (build\tidy-html5\tidy-5.4.0-w32-mt-XP\*) do move %%i build\tidy-html5\

for %%i in (Build Build\X64 Build\ARM64) do (
  for %%j in (Release Debug Test) do (
    mkdir %%i\%%j\Merge7z 2> NUL
    mkdir %%i\%%j\WinIMerge 2> NUL
    mkdir %%i\%%j\Frhed 2> NUL
    mkdir %%i\%%j\Filters 2> NUL
    mkdir %%i\%%j\ColorSchemes 2> NUL
    mkdir %%i\%%j\MergePlugins 2> NUL
    mkdir %%i\%%j\Commands\jq 2> NUL
    mkdir %%i\%%j\Commands\tidy-html5 2> NUL
    mkdir %%i\%%j\Commands\GnuWin32 2> NUL
    xcopy /s/y %%i\Merge7z %%i\%%j\Merge7z\
    xcopy /s/y %%i\Frhed %%i\%%j\Frhed\
    xcopy /s/y Build\GnuWin32 %%i\%%j\Commands\GnuWin32\
    copy Build\jq\jq-win32.exe %%i\%%j\Commands\jq\jq.exe
    copy Build\jq\jq-jq-1.4\COPYING %%i\%%j\Commands\jq\
    copy Build\tidy-html5\bin\tidy.* %%i\%%j\Commands\tidy-html5\
    copy Build\tidy-html5\tidy-html5-5.4.0\README\LICENSE.md %%i\%%j\Commands\tidy-html5\
    xcopy /s/y Plugins\Commands %%i\%%j\Commands
    xcopy /s/y Filters %%i\%%j\Filters\
    xcopy /s/y ColorSchemes %%i\%%j\ColorSchemes\
    xcopy /s/y Plugins\dlls\*.sct %%i\%%j\MergePlugins\
    xcopy /s/y Plugins\Plugins.xml %%i\%%j\MergePlugins\
    if "%%i" == "Build" (
      copy Build\WinIMerge\bin\WinIMergeLib.dll %%i\%%j\WinIMerge\
      copy Plugins\dlls\*.dll %%i\%%j\MergePlugins\
    ) else if "%%i" == "Build\X64" (
      copy Build\WinIMerge\bin64\WinIMergeLib.dll %%i\%%j\WinIMerge\
      copy Plugins\dlls\X64\*.dll %%i\%%j\MergePlugins\
    ) else if "%%i" == "Build\ARM64" (
      copy Build\WinIMerge\binARM64\WinIMergeLib.dll %%i\%%j\WinIMerge\
      copy Plugins\dlls\ARM64\*.dll %%i\%%j\MergePlugins\
    )
  )
)

popd
