cd /d "%~dp0"

del /s Build\*.exe

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

devenv.exe /build "Release" WinMerge.sln
devenv.exe /build "Release Unicode" WinMerge.sln
endlocal

setlocal
call "%VS100COMNTOOLS%vsvars32.bat"
devenv.exe /build "Release Unicode|Win32" WinMerge_vc10.sln
devenv.exe /build "Release Unicode|x64" WinMerge_vc10.sln
endlocal


"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMerge_vc71.iss"
"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMerge.iss"
"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMergeX64.iss"

pause
