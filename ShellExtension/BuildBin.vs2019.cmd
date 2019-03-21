cd /d "%~dp0"

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  call "%InstallDir%\Common7\Tools\vsdevcmd.bat
)

if "%1" == "" (
  call :BuildBin
  call :BuildBin x64
) else (
  call :BuildBin %1 
)

goto :eof

:BuildBin
set PLATFORM=%1
if "%1" == "" (
  set PLATFORM_VS=Win32
  set DLLFILENAME=ShellExtensionU.dll
) else (
  set PLATFORM_VS=%1
  set DLLFILENAME=ShellExtension%1.dll
)
MSBuild ShellExtension.vs2019.sln /t:Rebuild /p:Configuration="Unicode Release MinDependency" /p:Platform="%PLATFORM_VS%" || pause
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" "Build\%PLATFORM%\ShellExtensionUnicode Release MinDependency\%DLLFILENAME%"
)

goto :eof
