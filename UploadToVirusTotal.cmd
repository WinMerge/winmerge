cd /d "%~dp0"

set DISTDIR=.\Build\Releases

for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  "%ProgramFiles(x86)%\VirusTotalUploader2\VirusTotalUploader2.2.exe" %%f
)

