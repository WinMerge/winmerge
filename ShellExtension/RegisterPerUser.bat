@echo off
: RegisterPerUser.bat
:
: ShellExtension.dll is a so called 'Shell Extension'
: allowing user to start WinMerge from explorer's context
: menu to compare selected files/directories.
: However this dll file must be registered for
: Windows before it works.
:
: ShellExtension.dll can be unregistered with command:
: "RegisterPerUser.bat /U"

IF "%OS%" == ""                          Set DLLPATH=%0\..\ShellExtension.dll
IF "%PROCESSOR_ARCHITECTURE%" == "x86"   Set DLLPATH=%~dp0%ShellExtensionU.dll
IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" Set DLLPATH=%~dp0%ShellExtensionX64.dll
IF "%PROCESSOR_ARCHITECTURE%" == "IA64"  Set DLLPATH=%~dp0%ShellExtensionIA64.dll
IF "%PROCESSOR_ARCHITECTURE%" == "ARM64" Set DLLPATH=%~dp0%ShellExtensionARM64.dll
IF "%PROCESSOR_ARCHITECTURE%" == "ARM" Set DLLPATH=%~dp0%ShellExtensionARM.dll

SET OPTIONS=
FOR %%i IN (%*) DO (
  IF "%%i" == "/s" SET OPTIONS=%OPTIONS% /s 
  IF "%%i" == "/S" SET OPTIONS=%OPTIONS% /s 
)

FOR %%i IN (%*) DO (
  IF "%%i" == "/u" Goto Uninstall
  IF "%%i" == "/U" Goto Uninstall
)

Echo Registering ShellExtension.dll for current user only...
%WINDIR%\System32\RegSvr32 /n %OPTIONS% /i:user "%DLLPATH%"
IF EXIST "%~dp0\WinMerge32BitPluginProxy.exe" "%~dp0\WinMerge32BitPluginProxy.exe" /RegServerPerUser
Goto End

:Uninstall
Echo UnRegistering ShellExtension.dll for current user only...
%WINDIR%\System32\RegSvr32 /u /n %OPTIONS% /i:user "%DLLPATH%"
IF EXIST "%~dp0\WinMerge32BitPluginProxy.exe" "%~dp0\WinMerge32BitPluginProxy.exe" /UnregServerPerUser
Goto End

:End
