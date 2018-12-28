cd /d "%~dp0"

call SetVersion.cmd
set DISTDIR=.\build\Releases

for /F "delims=" %%f in ('type "%DISTDIR%\files.txt"') do (
  curl -u sdottaka:%BITBUCKET_PASSWORD% -X POST "https://api.bitbucket.org/2.0/repositories/sdottaka/winmerge-v2/downloads" --form files=@"%%f"
)
