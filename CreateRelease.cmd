set workdir=BuildTmp\Src
if "%1" == "vs2017" (
  set vsversion=vs2017
) else (
  set vsversion=vs2019x64_vs2017Win32
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

for /F %%f in (Build\Releases\files.txt) do (
  copy %%f ..\..\Build\Releases\
)
copy Build\Releases\files.txt ..\..\Build\Releases\
for /d %%d in (Build\Release\?.*.*) do (
  mkdir ..\..\Build\Releases\PDB\%%~nxd\Win32 2> NUL
  xcopy /y /s %%d ..\..\Build\Releases\PDB\%%~nxd\Win32\
)
for /d %%d in (Build\x64\Release\?.*.*) do (
  mkdir ..\..\Build\Releases\PDB\%%~nxd\x64 2> NUL
  xcopy /y /s %%d ..\..\Build\Releases\PDB\%%~nxd\x64\
)
for /d %%d in (Build\ARM64\Release\?.*.*) do (
  mkdir ..\..\Build\Releases\PDB\%%~nxd\ARM64 2> NUL
  xcopy /y /s %%d ..\..\Build\Releases\PDB\%%~nxd\ARM64\
)
popd

call UploadToVirusTotal.cmd

popd

