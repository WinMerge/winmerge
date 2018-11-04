cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=c:\web page\geocities

set GITHUB_USER=sdottaka
set GITHUB_REPO=winmerge-v2-jp

(
@echo off
echo virustotal.com's result:
echo.
for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  for /F %%h in ('certutil -hashfile %%f SHA256 ^| findstr -v hash') do echo %%~nxf: https://www.virustotal.com/en/file/%%h/analysis/
)
@echo on
) > _tmp_.txt

type _tmp_.txt | c:\tools\github-release release --tag %APPVER% --name "WinMerge %MAJOR%.%MINOR%.%REVISION% + (3-way merge, image compare/merge) +-jp-%PATCHLEVEL%" --description - --pre-release 

del _tmp_.txt

for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  c:\tools\github-release upload -t %APPVER% -n %%~nxf -f %%f
)

