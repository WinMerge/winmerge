pushd "%~dp0"

setlocal
set PATH="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%PATH%
set DISTDIR=..\Build\Releases

mkdir ..\Build\ShellExtension 2> NUL

copy /y "..\Build\ShellExtensionUnicode Release MinDependency\ShellExtensionU.dll" ..\Build\ShellExtension\
copy /y "..\Build\x64\ShellExtensionUnicode Release MinDependency\ShellExtensionX64.dll" ..\Build\ShellExtension\

WMIC Path CIM_DataFile WHERE Name='%CD:\=\\%\\..\\Build\\ShellExtension\\ShellExtensionX64.dll' Get Version | findstr /v Version > _tmp_.txt
set /P DLLVERSIONTMP=<_tmp_.txt
set DLLVERSION=%DLLVERSIONTMP: =%
del _tmp_.txt

7z.exe a -tzip "%DISTDIR%\ShellExtension-%DLLVERSION%.zip" ..\Build\ShellExtension

popd
goto :eof

