# Register.bat
#
# ShellExtension.dll is a so called 'Shell Extension'
# allowing user to start WinMerge from explorer's context
# menu to compare selected files/directories.
# However this dll file must be registered for
# Windows before it works.
#
# ShellExtension.dll can be unregistered with command:
# regsvr32 -u Shell_Extension.dll

echo Registering ShellExtension.dll...
regsvr32 ShellExtension.dll