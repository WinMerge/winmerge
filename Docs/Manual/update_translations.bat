@echo off
pushd %~dp0
if "%MSYS_HOME%" == "" (
  for %%i in (C D E F G) do (
    if exist %%i:\msys64\ set MSYS_HOME=%%i:\msys64
  )
)

(
echo #!/bin/bash
echo cd $^(cygpath '%CD%'^)
echo po4a --verbose --wrap-po newlines --package-name WinMerge --package-version 2.16 --copyright-holder 'Thingamahoochie Software' --msgid-bugs-address https://github.com/WinMerge/winmerge/issues ./Manual.cfg
) > %MSYS_HOME%\tmp\__update_translations__.sh
%MSYS_HOME%\usr\bin\bash.exe -lc /tmp/__update_translations__.sh
del %MSYS_HOME%\tmp\__update_translations__.sh
popd
