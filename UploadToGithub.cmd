cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=.\build\Releases
set GH_REPO=sdottaka/winmerge-v2-jp

(
@echo off
echo virustotal.com's result:
echo.
for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  for /F %%h in ('certutil -hashfile %%f SHA256 ^| findstr -v hash ^| findstr -v SHA') do echo %%~nxf: https://www.virustotal.com/en/file/%%h/analysis/
)
@echo on
) > _tmp_.txt

gh release create %APPVER% -p -F _tmp_.txt -t "WinMerge %MAJOR%.%MINOR%.%REVISION%+-jp-%PATCHLEVEL%" 

del _tmp_.txt

for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  gh release upload %APPVER% %%f
)

