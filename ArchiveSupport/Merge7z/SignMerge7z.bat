@echo off

call "%SIGNBAT_PATH%" "..\..\Build\Release\Merge7z\Merge7z.dll"
call "%SIGNBAT_PATH%" "..\..\Build\x64\Release\Merge7z\Merge7z.dll"

pause
