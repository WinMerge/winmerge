@echo off
rem This batch file calls the script to build the manual as CHM file.

pushd "../../Docs/Users/Manual/build/"
call build_htmlhelp
popd

@echo on