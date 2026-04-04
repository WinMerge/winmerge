@echo off
pushd "%~dp0"
echo Prepare Translations...
rmdir /S /Q "Build\Translations" 2> NUL
mkdir "Build\Translations\WinMerge" 2> NUL
mkdir "Build\Translations\ShellExtension" 2> NUL
copy Translations\WinMerge\*.po Build\Translations\WinMerge\ > NUL
copy Translations\ShellExtension\*.po Build\Translations\ShellExtension\ > NUL
powershell.exe -ExecutionPolicy RemoteSigned -File "Translations\StripPoComments.ps1" "Build\Translations"
popd
