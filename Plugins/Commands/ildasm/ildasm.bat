@echo off
setlocal enabledelayedexpansion
if exist "%APPDATA%\WinMerge\Commands\ildasm\ildasmpath.txt" (
  for /f "usebackq tokens=*" %%i in (%APPDATA%\WinMerge\Commands\ildasm\ildasmpath.txt) do set ILDASM_PATH=%%i
)
if not exist "!ILDASM_PATH!" (
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
  mkdir "%APPDATA%\WinMerge\Commands\ildasm\" 2> NUL
  where ildasm.exe > "%APPDATA%\WinMerge\Commands\ildasm\ildasmpath.txt"
  if exist "%APPDATA%\WinMerge\Commands\ildasm\ildasmpath.txt" (
    for /f "usebackq tokens=*" %%i in (%APPDATA%\WinMerge\Commands\ildasm\ildasmpath.txt) do set ILDASM_PATH=%%i
  )
)
"!ILDASM_PATH!" %*
