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
call "%VS71COMNTOOLS%vsvars32.bat"
set _ACP_ATLPROV=%VS71COMMONTOOLS%\..\..\Vc7\bin\ATLPROV.DLL

echo. > error.log
devenv.exe /rebuild "Release" WinMerge.sln /Out error.log
type error.log
echo. > error.log
devenv.exe /rebuild "Release Unicode" WinMerge.sln /Out error.log
type error.log
endlocal

setlocal
call "%VS100COMNTOOLS%vsvars32.bat"
MSBuild WinMerge_vc10.sln /t:Rebuild /p:Configuration="Release Unicode" /p:Platform="Win32"
MSBuild WinMerge_vc10.sln /t:Rebuild /p:Configuration="Release Unicode" /p:Platform="x64"
endlocal


"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMerge_vc71.iss"
"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMerge.iss"
"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMergeX64.iss"

pause
