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
RegSvr32 ShellExtension.dll

Goto End

:Uninstall
Echo UnRegistering ShellExtension.dll...
RegSvr32 /u ShellExtension.dll

:End
CLS