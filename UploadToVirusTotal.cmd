cd /d "%~dp0"

set DISTDIR=.\Build\Releases

call Tools\Scripts\URLFileSizeSHA256.cmd

for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  echo %%f | findstr /v /c:"-pdb.7z" && (call :upload %%f) && timeout 30
)

@echo off
echo.
for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  for /F %%h in ('certutil -hashfile %%f SHA256 ^| findstr -v SHA256 ^| findstr -v hash') do echo %%~nxf: https://www.virustotal.com/en/file/%%h/analysis/
)

goto :eof

:upload
if exist "%ProgramFiles(x86)%\VirusTotalUploader2\VirusTotalUploader2.2.exe" (
  "%ProgramFiles(x86)%\VirusTotalUploader2\VirusTotalUploader2.2.exe" %1
) else (
  "%ProgramFiles(x86)%\Samuel Tulach\VirusTotalUploader\uploader.exe" "%CD%\%~1"
)
goto :eof
