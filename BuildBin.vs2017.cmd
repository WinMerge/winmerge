cd /d "%~dp0"

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -version [15.0^,16.0^) -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
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
) else (
  set PLATFORM_VS=%1
)
MSBuild WinMerge.vs2017.sln /t:Rebuild /p:Configuration="Release" /p:Platform="%PLATFORM_VS%" || pause
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" Build\%PLATFORM%\Release\WinMergeU.exe
)

mkdir Build\%PLATFORM%\Release\%APPVER% 2> NUL
copy Build\%PlATFORM%\Release\*.pdb "Build\%PLATFORM%\Release\%APPVER%\"
goto :eof
