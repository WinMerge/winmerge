set workdir=BuildTmp\Src

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
call BuildAll.vs2017.cmd

mkdir ..\..\Build\Releases\PDB\%APPVER%\Win32 2> NUL
mkdir ..\..\Build\Releases\PDB\%APPVER%\x64 2> NUL
for /F %%f in (Build\Releases\files.txt) do (
  copy %%f ..\..\Build\Releases\
)
copy Build\Releases\files.txt ..\..\Build\Releases\
copy  Build\MergeUnicodeRelease\*.pdb ..\..\Build\Releases\PDB\%APPVER%\Win32\
copy  Build\x64\MergeUnicodeRelease\*.pdb ..\..\Build\Releases\PDB\%APPVER%\x64\
popd

call UploadToVirusTotal.cmd

popd

