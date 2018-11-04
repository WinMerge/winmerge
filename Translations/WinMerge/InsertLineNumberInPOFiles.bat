del "%~dp0English.pot"
cscript "%~dp0CreateMasterpotFile.vbs" /InsertLineNumbers:True
cscript "%~dp0UpdatePoFilesFromPotFile.vbs"
pause
