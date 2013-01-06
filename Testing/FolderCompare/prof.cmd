cd /d "%~dp0"
set PATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Team Tools\Performance Tools;%PATH%
rem set PATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Team Tools\Performance Tools\x64;%PATH%

rem VSInstr Release\FolderCompare.exe
VSPerfCmd /start:sample /output:FolderCompare.vsp
VSPerfCmd /launch:%~dp0\Release\FolderCompare.exe
VSPerfCmd /shutdown
VSPerfReport FolderCompare.vsp /summary:all

pause
