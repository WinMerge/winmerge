@echo off
setlocal EnableExtensions

rem Find msgcat.exe in known locations and execute it

set TOOL=msgcat.exe
set FOUND=

set "SEARCH_PATHS=%ProgramFiles%\Poedit\GettextTools\bin;%ProgramFiles(x86)%\Poedit\GettextTools\bin;C:\msys64\usr\bin"

for %%D in ("%SEARCH_PATHS:;=" "%") do (
    if exist "%%~D\%TOOL%" (
        set "FOUND=%%~D\%TOOL%"
        goto :run
    )
)

rem Tool not found
echo %TOOL% not found. Searched paths: 1>&2
for %%D in ("%SEARCH_PATHS:;=" "%") do echo   %%~D 1>&2
exit /b 1

:run
for %%I in ("%FOUND%") do set "PATH=%%~dpI"
"%FOUND%" %*
exit /b %ERRORLEVEL%
