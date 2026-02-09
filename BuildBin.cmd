cd /d "%~dp0"

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.js Version.in > Version.h

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  call "%InstallDir%\Common7\Tools\vsdevcmd.bat
)

if "%1" == "" (
  call :BuildBin ARM64 || goto :eof
  call :BuildBin x86|| goto :eof
  call :BuildBin x64 || goto :eof
) else (
  call :BuildBin %1 || goto :eof
)

goto :eof

:BuildBin
MSBuild WinMerge.sln /t:Rebuild /p:Configuration="Release" /p:Platform="%1" || goto :eof
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" Build\%1\Release\WinMergeU.exe
)

mkdir Build\%1\Release\%APPVER% 2> NUL
copy Build\%1\Release\*.pdb "Build\%1\Release\%APPVER%\"
goto :eof
