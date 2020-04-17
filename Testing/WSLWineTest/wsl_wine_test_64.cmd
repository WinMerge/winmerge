cd %~dp0
(tasklist | findstr vcxsrv) || "%ProgramFiles%\VcXsrv\xlaunch.exe" -run config.xlaunch || pause
bash -c "DISPLAY=:0 wine ../../Build/x64/Debug/WinMergeU"
