@echo off

call "%SIGNBAT_PATH%" dlls\*.dll
call "%SIGNBAT_PATH%" dlls\x64\*.dll

pause
