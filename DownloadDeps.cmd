setlocal
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%
set downloadsdir=%~dp0\build\WinMergeDownloadDeps
set urls_destdirs=^
https://bitbucket.org/winmerge/winmerge/downloads/winmerge_manual_build_tools_v1.zip!Docs\Users\Manual\build ^
https://bitbucket.org/winmerge/winmerge/downloads/ShellExtension-1.17.9.0.zip!Build ^
https://bitbucket.org/winmerge/winmerge/downloads/Merge7z1900.1-win32.zip!Build ^
https://bitbucket.org/winmerge/winmerge/downloads/Merge7z1900.1-x64.zip!Build\X64 ^
https://bitbucket.org/winmerge/frhed/downloads/frhed-0.10904.2017.1-win32.zip!Build ^
https://bitbucket.org/winmerge/frhed/downloads/frhed-0.10904.2017.1-x64.zip!Build\X64 ^
https://bitbucket.org/winmerge/winimerge/downloads/winimerge-1-0-10-0-exe.zip!Build ^
https://bitbucket.org/winmerge/patch/downloads/patch-2.5.9-7-bin.zip!Build\GnuWin32

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
