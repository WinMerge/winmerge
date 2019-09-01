set DISTDIR=.\build\Releases
set workdir=BuildTmp\Src
if "%1" == "vs2017" (
  set vsversion=vs2017
) else (
  set vsversion=vs2019
)

pushd "%~dp0"

rmdir /q /s %workdir% > NUL 2> NUL
mkdir %workdir% 2> NUL

git submodule init
git submodule update

git checkout-index -a -f --prefix=%workdir%\
for /d %%d in (Externals\*) do (
  pushd %%d
  if exist .git (
    rmdir /q /s ..\..\%workdir%\%%d
    mkdir ..\..\%workdir%\%%d
    git checkout-index -a -f --prefix=..\..\%workdir%\%%d\
  )
  popd
)

pushd %workdir%
call DownloadDeps.cmd
call BuildAll.%vsversion%.cmd

mkdir "%DISTDIR%\PDB\%APPVER%\Win32" 2> NUL
mkdir "%DISTDIR%\PDB\%APPVER%\x64" 2> NUL
for /F %%f in ("%DISTDIR%\files.txt") do (
  copy %%f "%DISTDIR%"
)
copy Build\Releases\files.txt ..\..\Build\Releases\
copy  Build\Release\*.pdb "%DISTDIR%\PDB\%APPVER%\Win32\"
copy  Build\x64\Release\*.pdb "%DISTDIR%\PDB\%APPVER%\x64\"
popd

call UploadToVirusTotal.cmd

popd

