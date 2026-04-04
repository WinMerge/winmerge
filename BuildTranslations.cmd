@echo off
pushd "%~dp0"
echo Prepare Translations...
rmdir /S /Q "BuildTmp\Translations" 2> NUL
mkdir "BuildTmp\Translations\WinMerge" 2> NUL
mkdir "BuildTmp\Translations\ShellExtension" 2> NUL
copy Translations\WinMerge\*.po BuildTmp\Translations\WinMerge\ > NUL
copy Translations\ShellExtension\*.po BuildTmp\Translations\ShellExtension\ > NUL
powershell.exe -ExecutionPolicy RemoteSigned -File "Translations\StripPoComments.ps1" "BuildTmp\Translations"
popd
