if "%1" == "" (
  call :BuildInstaller
  call :BuildInstaller x64
  call :BuildInstaller x64NonAdmin
  call :BuildInstaller ARM64
) else if "%1" == "x64" (
  call :BuildInstaller x64
  call :BuildInstaller x64NonAdmin
) else if "%1" == "ARM64" (
  call :BuildInstaller ARM64
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
