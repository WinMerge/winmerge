@echo off
rem This batch file calls the script to creates the master POT file.

pushd "../../Translations/WinMerge/"
cscript //nologo CreateMasterPotFile.vbs
echo.
cscript //nologo UpdatePoFilesFromPotFile.vbs
popd

@echo on