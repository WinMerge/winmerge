@echo off

call "%SIGNBAT_PATH%" "..\..\Build\MergeUnicodeRelease\Merge7z\Merge7z*.dll"
call "%SIGNBAT_PATH%" "..\..\Build\x64\MergeUnicodeRelease\Merge7z\Merge7z*.dll"

pause
