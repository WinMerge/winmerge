pushd "%~dp0"

setlocal
set MERGE7z_VERSION=1900.1
set PATH="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%PATH%
set DISTDIR=..\..\Build\Releases

if "%1" == "" (
  call :BuildZip 
  call :BuildZip x64
) else (
  call :BuildZip %1 
)

popd
goto :eof


:BuildZip

set PLATFORM=%1

mkdir ..\..\Build\%PLATFORM%\Merge7z\Lang 2> NUL
copy ..\..\Build\%PLATFORM%\Release\Merge7z\Merge7z.dll ..\..\Build\%PLATFORM%\Merge7z\
if "%PLATFORM%" == "" (
  set "ProgramFilesP=%ProgramFiles(x86)%"
  set ZIP_FILENAME=Merge7z%MERGE7Z_VERSION%-win32.zip
) else (
  set ProgramFilesP=%ProgramFiles%
  set ZIP_FILENAME=Merge7z%MERGE7Z_VERSION%-x64.zip
)

copy /y "%ProgramFilesP%\7-Zip\7z.dll" ..\..\Build\%PLATFORM%\Merge7z\
copy /y "%ProgramFilesP%\7-Zip\History.txt" ..\..\Build\%PLATFORM%\Merge7z\
copy /y "%ProgramFilesP%\7-Zip\License.txt" ..\..\Build\%PLATFORM%\Merge7z\
copy /y "%ProgramFilesP%\7-Zip\Lang\*.*" ..\..\Build\%PLATFORM%\Merge7z\Lang\

7z.exe a -tzip "%DISTDIR%\%ZIP_FILENAME%" ..\..\Build\%PLATFORM%\Merge7z

goto :eof
