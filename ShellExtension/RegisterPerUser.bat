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
IF "%PROCESSOR_ARCHITECTURE%" == "IA64"  Set DLLPATH=%~dp0%ShellExtensionX64.dll

IF "%1" == "/u" Goto Uninstall
IF "%1" == "/U" Goto Uninstall

Echo Registering ShellExtension.dll for current user only...
%WINDIR%\System32\RegSvr32 /n /i:user "%DLLPATH%"
IF EXIST "%~dp0\WinMerge32BitPluginProxy.exe" "%~dp0\WinMerge32BitPluginProxy.exe" /RegServerPerUser
Goto End

:Uninstall
Echo UnRegistering ShellExtension.dll for current user only...
%WINDIR%\System32\RegSvr32 /u /n /i:user "%DLLPATH%"
IF EXIST "%~dp0\WinMerge32BitPluginProxy.exe" "%~dp0\WinMerge32BitPluginProxy.exe" /UnregServerPerUser
Goto End

:End
