@echo off

if "%1" == "html" goto html
if "%1" == "htmlhelp" goto htmlhelp
if "%1" == "" goto default

echo Syntax: build [html OR htmlhelp]
goto end

:html
rem if Windows NT/2000/XP...
if "%OS%" == "Windows_NT" call build_html.bat
rem if Windows 9x...
if "%OS%" == "" command /E:32768 /C build_html.bat
goto end

:htmlhelp
echo htmlhelp
rem if Windows NT/2000/XP...
if "%OS%" == "Windows_NT" call build_htmlhelp.bat
rem if Windows 9x...
if "%OS%" == "" command /E:32768 /C build_htmlhelp.bat
goto end

:default
goto html

:end
@echo on