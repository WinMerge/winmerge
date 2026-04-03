if not exist "BuildTmp\Translations\WinMerge" mkdir "BuildTmp\Translations\WinMerge" 2> NUL
if not exist "BuildTmp\Translations\ShellExtension" mkdir "BuildTmp\Translations\ShellExtension" 2> NUL
copy Translations\WinMerge\*.po BuildTmp\Translations\WinMerge\ > NUL
copy Translations\ShellExtension\*.po BuildTmp\Translations\ShellExtension\ > NUL
powershell.exe -ExecutionPolicy RemoteSigned -File "%~dp0Translations\StripPoComments.ps1" "%~dp0BuildTmp\Translations"

if "%1" == "" (
  call :BuildInstaller x86
  call :BuildInstaller x64
  call :BuildInstaller x64NonAdmin
  call :BuildInstaller ARM64
) else if "%1" == "x64" (
  call :BuildInstaller x64
  call :BuildInstaller x64NonAdmin
) else if "%1" == "x86" (
  call :BuildInstaller x86
) else if "%1" == "ARM64" (
  call :BuildInstaller ARM64
) else if "%1" == "ARM" (
  echo platform %1 is not supported
) else (
  call :BuildInstaller %1
)
goto :eof

:BuildInstaller
set PLATFORM=%1

if "%PLATFORM%" == "ARM64" (
  for %%i in ("%LOCALAPPDATA%\Programs" "%ProgramFiles(x86)%" "%ProgramFiles%") do (
    if exist "%%~i\Inno Setup 6\iscc.exe" (
      "%%~i\Inno Setup 6\iscc.exe" "Installer\innosetup\WinMerge%PLATFORM%.is6.iss" || pause
      goto :eof
    ) else (
      if exist "%%~i\Inno Setup 5\iscc.exe" (
        "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge%PLATFORM%.iss" || pause
        goto :eof
      )
    )
  )
) else (
  echo.
  echo ============================================================
  echo Build WinMerge%PLATFORM%.iss with Inno Setup 5
  echo ============================================================
  for %%i in ("%ProgramFiles(x86)%" "%ProgramFiles%") do (
    if exist "%%~i\Inno Setup 5\iscc.exe" (
      "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge%PLATFORM%.iss" || pause
      goto :eof
    )
  )
)

goto :eof
