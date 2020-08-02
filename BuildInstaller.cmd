if "%1" == "" (
  call :BuildInstaller
  call :BuildInstaller x64
  call :BuildInstaller x64NonAdmin
) else (
  call :BuildInstaller %1 
)
goto :eof

:BuildInstaller
set PLATFORM=%1

rem if "%PLATFORM%" == "x64" (
rem   for %%i in ("%LOCALAPPDATA%\Programs" "%ProgramFiles(x86)%" "%ProgramFiles%") do (
rem     if exist "%%~i\Inno Setup 6\iscc.exe" (
rem       "%%~i\Inno Setup 6\iscc.exe" "Installer\innosetup\WinMerge%PLATFORM%.is6.iss" || pause
rem       goto :eof
rem     ) else (
rem       if exist "%%~i\Inno Setup 5\iscc.exe" (
rem         "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge%PLATFORM%.iss" || pause
rem         goto :eof
rem       )
rem     )
rem   )
rem ) else (
  for %%i in ("%ProgramFiles(x86)%" "%ProgramFiles%") do (
    if exist "%%~i\Inno Setup 5\iscc.exe" (
      "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge%PLATFORM%.iss" || pause
      goto :eof
    )
  )
rem )

goto :eof
