cd %~dp0..
powershell -c Add-AppxPackage %~dp0WinMergeContextMenuPackage.msix -ExternalLocation %CD%\Build\x64\Debug
pause
