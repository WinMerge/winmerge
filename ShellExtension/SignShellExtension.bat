@echo off

call "%SIGNBAT_PATH%" "..\Build\ShellExtensionUnicode Release MinDependency\ShellExtensionU.dll"
call "%SIGNBAT_PATH%" "..\Build\x64\ShellExtensionUnicode Release MinDependency\ShellExtensionX64.dll"

pause
