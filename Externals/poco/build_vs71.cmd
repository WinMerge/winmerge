@echo off
if defined VS71COMNTOOLS (
call "%VS71COMNTOOLS%\vsvars32.bat")
buildwin 71 build shared both Win32 samples
