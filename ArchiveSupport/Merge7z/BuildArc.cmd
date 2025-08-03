pushd "%~dp0"

setlocal
set MERGE7z_VERSION=2500.0
set PATH="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%PATH%
set DISTDIR=..\..\Build\Releases

if "%1" == "" (
  call :BuildZip x86
  call :BuildZip x64
  call :BuildZip ARM
  call :BuildZip ARM64
) else (
  call :BuildZip %1 
)

popd
goto :eof


:BuildZip

set PLATFORM=%1

mkdir %DISTDIR%\%PLATFORM%\Merge7z\Lang 2> NUL
copy ..\..\Build\%PLATFORM%\Release\Merge7z\Merge7z.dll %DISTDIR%\%PLATFORM%\Merge7z\
if "%PLATFORM%" == "x86" (
  set "ProgramFilesP=%ProgramFiles(x86)%"
) else (
  set ProgramFilesP=%ProgramFiles%
)
set ZIP_FILENAME=Merge7z%MERGE7Z_VERSION%-%PLATFORM%.zip

if "%PLATFORM%" == "ARM64" (
  copy /y "..\..\Externals\sevenzip\CPP\7zip\Bundles\Format7zF\arm64\7z.dll" %DISTDIR%\%PLATFORM%\Merge7z\
) else if "%PLATFORM%" == "ARM" (
  copy /y "..\..\Externals\sevenzip\CPP\7zip\Bundles\Format7zF\arm\7z.dll" %DISTDIR%\%PLATFORM%\Merge7z\
) else (
  copy /y "%ProgramFilesP%\7-Zip\7z.dll" %DISTDIR%\%PLATFORM%\Merge7z\
)
copy /y "%ProgramFilesP%\7-Zip\History.txt" %DISTDIR%\%PLATFORM%\Merge7z\
copy /y "%ProgramFilesP%\7-Zip\License.txt" %DISTDIR%\%PLATFORM%\Merge7z\
copy /y "%ProgramFilesP%\7-Zip\Lang\*.*" %DISTDIR%\%PLATFORM%\Merge7z\Lang\

7z.exe a -tzip "%DISTDIR%\%ZIP_FILENAME%" %DISTDIR%\%PLATFORM%\Merge7z

goto :eof
