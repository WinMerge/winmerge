@echo off
rem This batch file is used to build the Resource DLL

rem ### SET THIS TO THE PATH TO THE RC FILE FOR YOUR LANGUAGE ###
set rcScript=.\Dutch\MergeDutch.rc
set outputdir=.\DLL

.\MakeResDll -o %outputdir% %rcScript%
