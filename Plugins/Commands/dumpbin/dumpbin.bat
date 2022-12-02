@echo off
setlocal enabledelayedexpansion
if exist "%APPDATA%\WinMerge\Commands\dumpbin\dumpbinpath.txt" (
  for /f "usebackq tokens=*" %%i in (%APPDATA%\WinMerge\Commands\dumpbin\dumpbinpath.txt) do set DUMPBIN_PATH=%%i
)
if not exist "!DUMPBIN_PATH!" (
  if exist "%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" (
    for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -latest -products * -property installationPath`) do (
      set InstallDir=%%i
    )
  )
  if exist "!InstallDir!\Common7\Tools\vsdevcmd.bat" (
    call "!InstallDir!\Common7\Tools\vsdevcmd.bat" > NUL
  ) else (
    echo Visual Studio not installed
    goto :eof
  )
  mkdir "%APPDATA%\WinMerge\Commands\dumpbin\" 2> NUL
  where dumpbin.exe > "%APPDATA%\WinMerge\Commands\dumpbin\dumpbinpath.txt"
  if exist "%APPDATA%\WinMerge\Commands\dumpbin\dumpbinpath.txt" (
    for /f "usebackq tokens=*" %%i in (%APPDATA%\WinMerge\Commands\dumpbin\dumpbinpath.txt) do set DUMPBIN_PATH=%%i
  )
)
"!DUMPBIN_PATH!" %*
