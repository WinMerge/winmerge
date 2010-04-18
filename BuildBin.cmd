cd /d "%~dp0"

del /s Build\*.exe

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

setlocal
call "%VS71COMNTOOLS%vsvars32.bat"
set _ACP_ATLPROV=%VS71COMMONTOOLS%\..\..\Vc7\bin\ATLPROV.DLL

devenv.exe /build "Release" WinMerge.sln
devenv.exe /build "Release Unicode" WinMerge.sln
endlocal

setlocal
call "%VS90COMNTOOLS%vsvars32.bat"
devenv.exe /build "Release|x64" WinMergeX64.sln
devenv.exe /build "Release Unicode|x64" WinMergeX64.sln
endlocal

"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMerge.iss"
"\progra~1\inno setup 5\iscc" "Installer\innosetup\WinMergeX64.iss"

pause
