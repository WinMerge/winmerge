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
Call :Execute regsvr32 "%DLLPATH%"
IF EXIST "%~dp0\WinMerge32BitPluginProxy.exe" Call :Execute "%~dp0\WinMerge32BitPluginProxy.exe" /RegServer
Goto End

:Uninstall
Echo UnRegistering ShellExtension.dll...
IF "%OS%" == "" %WINDIR%\System\RegSvr32 /u "%DLLPATH%"
IF "%OS%" == "" Goto End
Call :Execute regsvr32 /u "%DLLPATH%"
IF EXIST "%~dp0\WinMerge32BitPluginProxy.exe" Call :Execute "%~dp0\WinMerge32BitPluginProxy.exe" /UnregServer
Goto End

:Execute
Ver | %WINDIR%\System32\Find "Version 5." > NUL
IF NOT ERRORLEVEL 1 (
  rem Windows 2000, XP, Sever 2003
  %1 %2 %3
) Else (
  rem Windows Vista, Server 200x, Server 201x, Windows 8, Windows 10
  Echo args="": For Each a in WScript.Arguments: If InStr^(a, " "^) ^> 0 Then args = args ^& """" ^& a ^& """ " Else args = args ^& a ^& " " End If: Next: CreateObject^("Shell.Application"^).ShellExecute "%~1", args, "", "runas" > "%TEMP%\Elevated.vbs"
  Wscript //nologo "%TEMP%\Elevated.vbs" %2 %3
  Del "%TEMP%\Elevated.vbs" 2> NUL
)
Goto :EOF

:End
CLS
