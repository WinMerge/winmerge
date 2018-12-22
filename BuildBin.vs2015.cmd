cd /d "%~dp0"

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

setlocal
set VisualStudioVersion=14.0
call "%VS140COMNTOOLS%vsvars32.bat"

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
MSBuild WinMerge.vs2015.sln /t:Rebuild /p:Configuration="Release Unicode" /p:Platform="%PLATFOMR_VS" || pause
endlocal

if exist "%SIGNBAT_PATH%" (
  call "%SIGNBAT_PATH%" Build\%PLATFORM%\MergeUnicodeRelease\WinMergeU.exe
  call "%SIGNBAT_PATH%" Build\%PLATFORM%\MergeUnicodeRelease\MergeLang.dll
)

mkdir Build\%PLATFORM%\MergeUnicodeRelease\%APPVER% 2> NUL
copy Build\%PlATFORM%\MergeUnicodeRelease\*.pdb "Build\%PLATFORM%\MergeUnicodeRelease\%APPVER%\"
goto :eof
