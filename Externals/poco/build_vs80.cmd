@echo off
if defined VS80COMNTOOLS (
call "%VS80COMNTOOLS%\vsvars32.bat")
buildwin 80 build shared both Win32 samples
