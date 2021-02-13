@echo off
rem This batch file calls the script to creates the master POT file.

pushd "../../Translations/WinMerge/"
cscript //nologo CreateMasterPotFile.vbs
echo.
powershell -executionpolicy remotesigned -file UpdatePoFilesFromPotFile.ps1
popd

@echo on
