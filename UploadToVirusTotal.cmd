cd /d "%~dp0"

set DISTDIR=c:\web page\geocities

for /F "delims=" %%f in ('type "%DISTDIR%\jp_files.txt"') do (
  "%ProgramFiles(x86)%\VirusTotalUploader2\VirusTotalUploader2.2.exe" %%f
)

