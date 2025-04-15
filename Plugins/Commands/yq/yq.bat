@echo off

set DOWNLOAD_URL=https://github.com/mikefarah/yq/releases/download/v4.11.1/yq_windows_386.exe
set YQ_PATH=Commands\yq\yq_windows_386.exe
set MESSAGE='yq command is not installed. Do you want to download it from %DOWNLOAD_URL%?'
set TITLE='YAML Data Querier Plugin'
set SHA256=fde958b4f5830d0cb878bedcb4a3155e4b269520ceeb33966d9b326fb5c62bb2

cd /d "%APPDATA%\WinMerge"
if not exist %YQ_PATH% (
  cd /d "%~dp0..\.."
  if not exist %YQ_PATH% (
    mkdir "%APPDATA%\WinMerge" 2> NUL
    cd /d "%APPDATA%\WinMerge"
    for %%i in (%YQ_PATH%) do mkdir %%~pi 2> NUL
    powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
    if errorlevel 1 (
      echo "download is canceled" 1>&2
    ) else (
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri %DOWNLOAD_URL% -UseBasicParsing -Outfile %YQ_PATH%"
      powershell -command "$(CertUtil -hashfile %YQ_PATH% SHA256)[1] -replace ' ','' -eq '%SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %YQ_PATH%: download failed 1>&2
        del %YQ_PATH% 2> NUL
      )
    )
  )
)
%YQ_PATH% %*
