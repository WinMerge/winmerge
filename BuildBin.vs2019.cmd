cd /d "%~dp0"

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -version [16.0^,17.0^) -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  call "%InstallDir%\Common7\Tools\vsdevcmd.bat
)

if "%1" == "" (
  call :BuildBin || goto :eof
  call :BuildBin ARM64 || goto :eof
  call :BuildBin x64 || goto :eof
) else (
  call :BuildBin %1 || goto :eof
)

goto :eof

:BuildBin
set PLATFORM=%1
if "%1" == "" (
  set PLATFORM_VS=Win32
) else (
  set PLATFORM_VS=%1
)
if "%PLATFORM_VS%" == "Win32" (
  set PLATFORM_DIR=
) else (
  set PLATFORM_DIR=%PLATFORM_VS%
)
MSBuild WinMerge.vs2019.sln /t:Rebuild /p:Configuration="Release" /p:Platform="%PLATFORM_VS%" || goto :eof
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" Build\%PLATFORM_DIR%\Release\WinMergeU.exe
)

mkdir Build\%PLATFORM_DIR%\Release\%APPVER% 2> NUL
copy Build\%PlATFORM_DIR%\Release\*.pdb "Build\%PLATFORM_DIR%\Release\%APPVER%\"
goto :eof
