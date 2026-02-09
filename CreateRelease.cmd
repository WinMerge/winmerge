for /f %%a in ('powershell -command "Get-Date -Format HH:mm:ss.fff"') do set start=%%a

set workdir=BuildTmp\Src
if "%1" == "vs2017" (
  set vsversion=vs2017
)
if "%1" == "latest" (
  set vsversion=vs2017_32bit_latest_64bit
) else (
  set vsversion=vs2017_32bit_vs2022_64bit
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

rem call UploadToVirusTotal.cmd

popd

for /f %%a in ('powershell -command "Get-Date -Format HH:mm:ss.fff"') do set end=%%a
for /f %%a in ('powershell -command "(New-TimeSpan -Start (Get-Date '%start%') -End (Get-Date '%end%')).TotalMilliseconds"') do set elapsed=%%a

echo %DATE% %start%,%end%,%elapsed%[ms] >> Build\Releases\buildtime.txt
