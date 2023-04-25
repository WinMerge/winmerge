cd /d "%~dp0"

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -version [16.0^,17.0^) -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  call "%InstallDir%\Common7\Tools\vsdevcmd.bat
)

if "%1" == "" (
  call :BuildBin x86 || goto :eof
  call :BuildBin x64 || goto :eof
  call :BuildBin ARM || goto :eof
  call :BuildBin ARM64 || goto :eof
) else (
  call :BuildBin %1 || goto :eof
)

pushd WinMerge32BitPluginProxy
MSBuild WinMerge32BitPluginProxy.vs2019.sln /t:Rebuild /p:Configuration="Release" /p:Platform="x86" || goto :eof
popd

endlocal

goto :eof

:BuildBin

pushd src_VCPP
MSBuild VCPPPlugins.vs2022.sln /t:Rebuild /p:Configuration="Release" /p:Platform="%1" || goto :eof
popd

mkdir dlls\%1 2> NUL
copy src_VCPP\Build\%1\Release\MergePlugins\*.dll dlls\%1\

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" %~dp0dlls\%1\*.dll
)

goto :eof
