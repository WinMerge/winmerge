@echo off
: Register.bat
:
: ShellExtension.dll is a so called 'Shell Extension'
: allowing user to start WinMerge from explorer's context
: menu to compare selected files/directories.
: However this dll file must be registered for
: Windows before it works.
:
: ShellExtension.dll can be unregistered with command:
: "Register.bat /U"

IF "%1" == "/u" Goto Uninstall
IF "%1" == "/U" Goto Uninstall

Echo Registering ShellExtension.dll...
if "%OS%" == "" RegSvr32 ShellExtension.dll
if "%OS%" == "Windows_NT" RegSvr32 ShellExtensionU.dll

Goto End

:Uninstall
Echo UnRegistering ShellExtension.dll...
if "%OS%" == "" RegSvr32 /u ShellExtension.dll
if "%OS%" == "Windows_NT" RegSvr32 /u ShellExtensionU.dll

:End
CLS