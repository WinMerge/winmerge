cd /d "%~dp0"

del /s Build\*.exe
del /s BuildTmp\*.res

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

setlocal
for /f "usebackq tokens=*" %%i in (`"%programfiles(x86)%\microsoft visual studio\installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)
if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  call "%InstallDir%\Common7\Tools\vsdevcmd.bat %*
)
MSBuild WinMerge.sln /t:Rebuild /p:Configuration="Release Unicode" /p:Platform="Win32" || pause
MSBuild WinMerge.sln /t:Rebuild /p:Configuration="Release Unicode" /p:Platform="x64" || pause
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" Build\MergeUnicodeRelease\WinMergeU.exe
  call "%SIGNBAT_PATH%" Build\MergeUnicodeRelease\MergeLang.dll
  call "%SIGNBAT_PATH%" Build\x64\MergeUnicodeRelease\WinMergeU.exe
  call "%SIGNBAT_PATH%" Build\x64\MergeUnicodeRelease\MergeLang.dll
)

mkdir Build\MergeUnicodeRelease\%APPVER% 2> NUL
mkdir Build\x64\MergeUnicodeRelease\%APPVER% 2> NUL
copy Build\MergeUnicodeRelease\*.pdb "Build\MergeUnicodeRelease\%APPVER%\"
copy Build\x64\MergeUnicodeRelease\*.pdb "Build\x64\MergeUnicodeRelease\%APPVER%\"

for %%i in ("%ProgramFiles(x86)%" "%ProgramFiles%") do (
  if exist "%%~i\Inno Setup 5\iscc.exe" (
    "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMerge.iss" || pause
    "%%~i\Inno Setup 5\iscc.exe" "Installer\innosetup\WinMergeX64.iss" || pause
  )
)

