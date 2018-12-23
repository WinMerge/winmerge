mkdir Build\Docs 2> NUL

cd Docs\Users\Manual\build
call build_htmlhelp.bat
cd ..\..\..\..
cd Docs\Users\ManualJapanese\build
call build_htmlhelp.bat
cd ..\..\..\..
