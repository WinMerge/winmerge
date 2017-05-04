cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=c:\web page\geocities

set GITHUB_USER=sdottaka
set GITHUB_REPO=winmerge-v2

c:\tools\github-release release -t %APPVER% -n "WinMerge %MAJOR%.%MINOR%.%REVISION% + (3-way merge, image compare/merge) +-jp-%PATCHLEVEL%" -p

for /F "delims=" %%f in ('type "%DISTDIR%\jp_files.txt"') do (
  c:\tools\github-release upload -t %APPVER% -n %%~nxf -f %%f
)

