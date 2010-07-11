mkdir Build\Docs 2> NUL
for %%i in (Docs\Users\*.txt Src\COPYING) do (
  cscript convertlf2crlf.vbs "%%i" Build\Docs\%%~nxi"
)

for %%i in (Translations\Docs\Readme\Readme-Japanese.txt) do (
  cscript convertlf2crlf.vbs "%%i" Build\Docs\%%~nxi"
)

cd Docs\Users\Manual\build
call build_htmlhelp.bat
cd ..\..\..\..
cd Docs\Users\Manual_ja\build
call build_htmlhelp.bat
cd ..\..\..\..
