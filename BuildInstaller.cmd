if "%1" == "" (
  call :BuildInstaller
  call :BuildInstaller x64
) else (
  call :BuildInstaller %1 
)
goto :eof

:BuildInstaller
set PLATFORM=%1

for %%i in ("%ProgramFiles(x86)%" "%ProgramFiles%") do (
  if exist "%%~i\Inno Setup 5\iscc.exe" (
    "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge%PLATFORM%.iss" || pause
  )
)
goto :eof
