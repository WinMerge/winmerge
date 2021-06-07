@echo off
set DOWNLOAD_URL=https://ftp.riken.jp/net/apache/tika/2.0.0-BETA/tika-app-2.0.0-BETA.jar
set TIKA_PATH=WinMerge\Commands\Apache-Tika\tika-app-2.0.0-BETA.jar
set MESSAGE='Apache Tika is not installed. Do you want to download it from %DOWNLOAD_URL%?'
set TITLE='Apache Tika Plugin'

cd "%APPDATA%"
if not exist %TIKA_PATH% (
  for %%i in (%TIKA_PATH%) do mkdir %%~pi 2> NUL
  powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
  if errorlevel 1 (
    echo "download is canceled" 1>&2
  ) else (
    start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL% -Outfile %TIKA_PATH%"
  )
)
java -jar %TIKA_PATH% --encoding=UTF-8 %1 "%~2" > "%~3"
