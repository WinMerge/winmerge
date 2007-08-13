@echo off
rem This batch file calls the script to creates the master POT file.

pushd "../../Src/Languages/"
cscript //nologo CreateMasterPotFile.vbs
popd

@echo on