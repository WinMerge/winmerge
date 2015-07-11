mkdir Build\Docs 2> NUL

cd Docs\Users\Manual\build
call build_htmlhelp.bat
cd ..\..\..\..
cd Docs\Users\Manual_ja\build
call build_htmlhelp.bat
cd ..\..\..\..
