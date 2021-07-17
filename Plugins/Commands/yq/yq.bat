@echo off

set DOWNLOAD_URL=https://github.com/mikefarah/yq/releases/download/v4.9.7/yq_windows_386.exe
set YQ_PATH=Commands\yq\yq_windows_386.exe
set MESSAGE='yq command is not installed. Do you want to download it from %DOWNLOAD_URL%?'
set TITLE='YAML Data Querier Plugin'

cd "%APPDATA%\WinMerge"
if not exist %YQ_PATH% (
  cd "%~dp0..\.."
  if not exist %YQ_PATH% (
    cd "%APPDATA%\WinMerge"
    for %%i in (%YQ_PATH%) do mkdir %%~pi 2> NUL
    powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
    if errorlevel 1 (
      echo "download is canceled" 1>&2
    ) else (
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL% -Outfile %YQ_PATH%"
    )
  )
)
%YQ_PATH% %*
