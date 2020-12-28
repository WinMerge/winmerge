setlocal
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%
set downloadsdir=%~dp0\build\WinMergeDownloadDeps
set urls_destdirs=^
https://github.com/WinMerge/winmerge/releases/download/winmerge_manual_another_build_tools_v2/winmerge_manual_another_build_tools_v2.zip!Docs\Manual\Tools ^
https://github.com/WinMerge/winmerge/releases/download/ShellExtension-1.17.13.0/ShellExtension-1.17.13.0.zip!Build ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z1900.2/Merge7z1900.2-win32.zip!Build ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z1900.2/Merge7z1900.2-x64.zip!Build\X64 ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.5-win32.zip!Build ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.5-x64.zip!Build\X64 ^
https://github.com/WinMerge/winimerge/releases/download/1.0.22.0/winimerge-1-0-22-0-exe.zip!Build ^
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

popd
