cd /d "%~dp0"

call "%VS71COMNTOOLS%vsvars32.bat"
call "C:\Program Files\Microsoft Platform SDK\SetEnv.Cmd" /XP64 /RETAIL
devenv.exe /useenv

