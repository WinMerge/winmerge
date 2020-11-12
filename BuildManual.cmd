pushd Translations
for %%i in ("%LOCALAPPDATA%\Programs\Python\Python39" "C:\Python39") do (
  if exist "%%~i\python.exe" (
    "%%~i\python.exe" GetTranslationsStatus.py
  )
) 
popd

call Docs\Manual\build_htmlhelp_en.bat
call Docs\Manual\build_htmlhelp_jp.bat
