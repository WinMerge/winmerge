cd /d "%~dp0\ShellExtension"

call "%VS71COMNTOOLS%vsvars32.bat"
call "C:\Program Files\Microsoft Platform SDK\SetEnv.Cmd" /XP64 /RETAIL
set
devenv.exe /useenv ShellExtensionX64.sln /build "Release X64"

pause


