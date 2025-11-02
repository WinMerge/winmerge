cd /d "%~dp0"

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  call "%InstallDir%\Common7\Tools\vsdevcmd.bat
)

if "%1" == "" (
  call :BuildBin x86
  call :BuildBin x64
  call :BuildBin ARM
  call :BuildBin ARM64
) else (
  call :BuildBin %1 
)

goto :eof

:BuildBin
set PLATFORM=%1
if "%1" == "x86" (
  set PLATFORM_VS=x86
  set DLLFILENAME=ShellExtensionU.dll
) else (
  set PLATFORM_VS=%1
  set DLLFILENAME=ShellExtension%1.dll
)
MSBuild ShellExtension.sln /t:Rebuild /p:Configuration="Release" /p:Platform="%PLATFORM_VS%" || pause
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" "..\Build\%PLATFORM%\Release\%DLLFILENAME%"
  call "%SIGNBAT_PATH%" "..\Build\%PLATFORM%\Release\WinMergeContextMenu.dll"
)

goto :eof
