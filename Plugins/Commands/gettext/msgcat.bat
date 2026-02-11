@echo off
setlocal EnableExtensions

rem Find msgcat.exe in known locations and execute it

set TOOL=msgcat.exe
set FOUND=

where %TOOL% >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set FOUND=%TOOL%
    goto :run
)

set "SEARCH_PATHS=%ProgramFiles%\Poedit\GettextTools\bin;%ProgramFiles(x86)%\Poedit\GettextTools\bin;%LocalAppData%\Atlassian\SourceTree\git_local\usr\bin;C:\msys64\usr\bin"

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
"%FOUND%" %*
exit /b %ERRORLEVEL%
