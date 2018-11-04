set DISTDIR=\Web Page\geocities
set workdir=BuildTmp\Src
if "%1" == "vs2017" (
  set vsversion=vs2017
) else (
  set vsversion=vs2015
)

pushd "%~dp0"

rmdir /q /s %workdir% > NUL 2> NUL
mkdir %workdir% 2> NUL

call Externals\hg_clone.cmd

hg archive %workdir%
for /d %%d in (Externals\*) do (
  pushd %%d
  if exist .hg hg archive ..\..\%workdir%\%%d 
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
copy  Build\MergeUnicodeRelease\*.pdb "%DISTDIR%\PDB\%APPVER%\Win32\"
copy  Build\x64\MergeUnicodeRelease\*.pdb "%DISTDIR%\PDB\%APPVER%\x64\"
popd

call UploadToVirusTotal.cmd

popd

