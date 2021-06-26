@echo off
if not "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
  echo QueryCSV and QueryTSV plugins are only supported on x64 systems
  goto :eof
)

set DOWNLOAD_URL=https://github.com/harelba/q/releases/download/2.0.19/q-AMD64-Windows.exe
set Q_PATH=WinMerge\Commands\q\q-AMD64-Windows.exe
set MESSAGE='q command is not installed. Do you want to download it from %DOWNLOAD_URL%?'
set TITLE='CSV/TSV Data Querier Plugin'

cd "%APPDATA%"
if not exist %Q_PATH% (
  for %%i in (%Q_PATH%) do mkdir %%~pi 2> NUL
  powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
  if errorlevel 1 (
    echo "download is canceled" 1>&2
  ) else (
    start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL% -Outfile %Q_PATH%"
  )
)
%Q_PATH% %*
