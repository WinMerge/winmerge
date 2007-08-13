@echo off
rem This batch file calls the script to creates the language RC files from the
rem language PO files.

pushd "../../Src/Languages/"
cscript //nologo CreateRcFilesFromPoFiles.vbs
popd

@echo on