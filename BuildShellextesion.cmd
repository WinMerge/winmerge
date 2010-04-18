cd /d "%~dp0\ShellExtension"

call "%VS71COMNTOOLS%vsvars32.bat"
devenv.exe /useenv ShellExtension.sln /build "Release MinDependency"
devenv.exe /useenv ShellExtension.sln /build "Unicode Release MinDependency"

pause


