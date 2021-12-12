cd /d "%~dp0"

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -version [16.0^,17.0^) -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
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
MSBuild Merge7z.vs2019.sln /t:Rebuild /p:Configuration="Release" /p:Platform="%1" || pause
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" ..\..\Build\%1\Release\Merge7z\Merge7z.dll
)

mkdir ..\..\Build\%1\Release\%APPVER% 2> NUL
copy ..\..\Build\%1\Release\Merge7z\*.pdb "Build\%1\Release\%APPVER%\"
goto :eof
