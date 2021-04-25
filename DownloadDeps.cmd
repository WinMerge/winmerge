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
https://github.com/WinMerge/winimerge/releases/download/v1.0.27/winimerge-1.0.27.0-exe.zip!Build ^
https://github.com/WinMerge/patch/releases/download/v2.5.9-7/patch-2.5.9-7-bin.zip!Build\GnuWin32 ^
http://www.magicnotes.com/steelbytes/SBAppLocale_ENG.zip!Docs\Manual\Tools

pushd "%~dp0"
mkdir %downloadsdir% 2> NUL

for %%p in (%urls_destdirs%) do (
  for /F "tokens=1,2 delims=!" %%u in ("%%p") do (
    if not exist %downloadsdir%\%%~nxu (
      powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %%u -Outfile %downloadsdir%\%%~nxu"
    )
    7z x %downloadsdir%\%%~nxu -aoa -o%%v
  )
)

for %%i in (Build Build\X64 Build\ARM64) do (
  for %%j in (Release Debug Test) do (
    mkdir %%i\%%j\Merge7z 2> NUL
    mkdir %%i\%%j\WinIMerge 2> NUL
    mkdir %%i\%%j\Frhed 2> NUL
    mkdir %%i\%%j\Filters 2> NUL
    mkdir %%i\%%j\ColorSchemes 2> NUL
    mkdir %%i\%%j\MergePlugins 2> NUL
    xcopy /s/y %%i\Merge7z %%i\%%j\Merge7z\
    xcopy /s/y %%i\Frhed %%i\%%j\Frhed\
    xcopy /s/y Filters %%i\%%j\Filters\
    xcopy /s/y ColorSchemes %%i\%%j\ColorSchemes\
    xcopy /s/y Plugins\dlls\*.sct %%i\%%j\MergePlugins\
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
