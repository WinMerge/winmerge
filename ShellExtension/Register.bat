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
IF "%OS%" == "" RegSvr32 ShellExtension.dll
IF "%OS%" == "Windows_NT" (
  IF "%PROCESSOR_ARCHITECTURE%" == "x86"   RegSvr32 ShellExtensionU.dll
  IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" RegSvr32 ShellExtensionX64.dll
  IF "%PROCESSOR_ARCHITECTURE%" == "IA64"  RegSvr32 ShellExtensionX64.dll
)

Goto End

:Uninstall
Echo UnRegistering ShellExtension.dll...
IF "%OS%" == "" RegSvr32 /u ShellExtension.dll
IF "%OS%" == "Windows_NT" (
  IF "%PROCESSOR_ARCHITECTURE%" == "x86"   RegSvr32 /u ShellExtensionU.dll
  IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" RegSvr32 /u ShellExtensionX64.dll
  IF "%PROCESSOR_ARCHITECTURE%" == "IA64"  RegSvr32 /u ShellExtensionX64.dll
)

:End
CLS