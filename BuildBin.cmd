cd /d "%~dp0"

del /s Build\*.exe
del /s BuildTmp\*.res

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

rem Filters
mkdir Build\Filters 2> NUL
for %%i in (Filters\*.flt Filters\*.tmpl Filters\*.txt) do (
  cscript convertlf2crlf.vbs "%%i" Build\Filters\%%~nxi"
)

setlocal
call "%VS100COMNTOOLS%vsvars32.bat"
MSBuild WinMerge_vc10.sln /t:Rebuild /p:Configuration="Release Unicode" /p:Platform="Win32" || pause
MSBuild WinMerge_vc10.sln /t:Rebuild /p:Configuration="Release Unicode" /p:Platform="x64" || pause
endlocal

for %%i in ("%ProgramFiles(x86)%" "%ProgramFiles%") do (
  if exist "%%~i\Inno Setup 5\iscc.exe" (
    "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge.iss" || pause
    "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMergeX64.iss" || pause
  )
)

