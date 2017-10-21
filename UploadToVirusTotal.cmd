cd /d "%~dp0"

set DISTDIR=.\Build\Releases

for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  "%ProgramFiles(x86)%\VirusTotalUploader2\VirusTotalUploader2.2.exe" %%f
)

@echo off
echo.
for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  for /F %%h in ('certutil -hashfile %%f SHA256 ^| findstr -v hash') do echo %%~nxf: https://www.virustotal.com/en/file/%%h/analysis/
)

