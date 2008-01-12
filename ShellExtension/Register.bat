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

IF "%OS%" == ""                          Set DLLPATH=%0\..\ShellExtension.dll
IF "%PROCESSOR_ARCHITECTURE%" == "x86"   Set DLLPATH=%~dp0%ShellExtensionU.dll
IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" Set DLLPATH=%~dp0%ShellExtensionX64.dll
IF "%PROCESSOR_ARCHITECTURE%" == "IA64"  Set DLLPATH=%~dp0%ShellExtensionX64.dll

IF "%1" == "/u" Goto Uninstall
IF "%1" == "/U" Goto Uninstall

Echo Registering ShellExtension.dll...
IF "%OS%" == "" %WINDIR%\System\RegSvr32 "%DLLPATH%"
IF "%OS%" == "" Goto End
Call :ExecuteRegSvr32 "%DLLPATH%"
Goto End

:Uninstall
Echo UnRegistering ShellExtension.dll...
IF "%OS%" == "" %WINDIR%\System\RegSvr32 /u "%DLLPATH%"
IF "%OS%" == "" Goto End
Call :ExecuteRegSvr32 /u "%DLLPATH%"
Goto End

:ExecuteRegSvr32
Ver | Find "Version 6." > NUL
IF NOT ERRORLEVEL 1 (
  rem Windows Vista, Server 2008?
  Echo args="": For Each a in WScript.Arguments: args = args ^& """" ^& a ^& """ ": Next: CreateObject^("Shell.Application"^).ShellExecute "RegSvr32", args, "", "runas" > "%TEMP%\RegSvr32Elevated.vbs"
  Wscript //nologo "%TEMP%\RegSvr32Elevated.vbs" %*
  Del "%TEMP%\RegSvr32Elevated.vbs" 2> NUL
) Else (
  rem Windows NT4.0, 2000, XP, Sever 2003
  RegSvr32 %*
)
Goto :EOF

:End
CLS
