del "%~dp0English.pot"
cscript "%~dp0CreateMasterpotFile.js" /InsertLineNumbers:True
powershell -executionpolicy remotesigned -file "%~dp0UpdatePoFilesFromPotFile.ps1"
pause
