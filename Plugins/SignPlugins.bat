@echo off

call "%SIGNBAT_PATH%" dlls\*.dll
call "%SIGNBAT_PATH%" dlls\x64\*.dll
call "%SIGNBAT_PATH%" WinMerge32BitPluginProxy\Release\*.exe
pause
