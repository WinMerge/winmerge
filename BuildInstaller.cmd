
for %%i in ("%ProgramFiles(x86)%" "%ProgramFiles%") do (
  if exist "%%~i\Inno Setup 5\iscc.exe" (
    "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge.iss" || pause
    "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMergeX64.iss" || pause
  )
)
