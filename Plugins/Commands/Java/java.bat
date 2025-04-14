@echo off
where /q java.exe
if %ERRORLEVEL% == 0 (
  java %*
  goto :eof
)

setlocal EnableDelayedExpansion
set OpenJDKVer=19.0.2
set DOWNLOAD_URL=https://download.java.net/java/GA/jdk%OpenJDKVer%/fdb695a9d9064ad6b064dc6df578380c/7/GPL/openjdk-%OpenJDKVer%_windows-x64_bin.zip
set DOWNLOAD_DIR=Commands\Java
set DOWNLOAD_PATH=Commands\Java\openjdk-%OpenJDKVer%_windows-x64_bin.zip
set OPENJDK_JAVA_PATH=Commands\Java\jdk-%OpenJDKVer%\bin\java.exe
set MESSAGE='OpenJDK is not installed. Do you want to download it from %DOWNLOAD_URL%'
set TITLE='OpenJDK'
set OPENJDK_SHA256=9f70eba3f2631674a2d7d3aa01150d697f68be16ad76662ff948d7fe1b4985d8

cd /d "%APPDATA%\WinMerge"
if not exist %OPENJDK_JAVA_PATH% (
  cd /d "%~dp0..\.."
  if not exist %OPENJDK_JAVA_PATH% (
    mkdir "%APPDATA%\WinMerge" 2> NUL
    pushd "%APPDATA%\WinMerge"
    for %%i in (%OPENJDK_JAVA_PATH%) do mkdir %%~pi 2> NUL
    powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
    if errorlevel 1 (
      echo "download is canceled" 1>&2
    ) else (
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri %DOWNLOAD_URL% -UseBasicParsing -Outfile %DOWNLOAD_PATH%"
      powershell -command "$(CertUtil -hashfile '%DOWNLOAD_PATH%' SHA256)[1] -replace ' ','' -eq '%OPENJDK_SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %DOWNLOAD_PATH%: download failed 1>&2
      ) else (
        start "Extracting..." /WAIT powershell -command "Expand-Archive -Path %DOWNLOAD_PATH%" -DestinationPath %DOWNLOAD_DIR%"
      )
      del %DOWNLOAD_PATH% 2> NUL
    )
  )
)
"%OPENJDK_JAVA_PATH%" %*
