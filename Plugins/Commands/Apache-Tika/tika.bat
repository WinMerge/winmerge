@echo off
setlocal EnableDelayedExpansion
set TikaVer=2.2.1
set TikaJar=tika-app-%TikaVer%.jar
set DOWNLOAD_URL=https://repo1.maven.org/maven2/org/apache/tika/tika-app/%TikaVer%/%TikaJar%
set TIKA_PATH=Commands\Apache-Tika\%TikaJar%
set MESSAGE='Apache Tika is not installed. Do you want to download it and its dependences from %DOWNLOAD_URL%'
set TITLE='Apache Tika Plugin'
set TIKA_SHA256=9fc945031d45f1601f5cd55a560b412a88a5bb66b909506be0cc8110a52ffdf0

cd "%APPDATA%\WinMerge"
if not exist %TIKA_PATH% (
  cd "%~dp0..\.."
  if not exist %TIKA_PATH% (
    mkdir "%APPDATA%\WinMerge" 2> NUL
    cd "%APPDATA%\WinMerge"
    for %%i in (%TIKA_PATH%) do mkdir %%~pi 2> NUL
    powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
    if errorlevel 1 (
      echo "download is canceled" 1>&2
    ) else (
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL% -Outfile %TIKA_PATH%"
      powershell -command "$(CertUtil -hashfile '%TIKA_PATH%' SHA256)[1] -replace ' ','' -eq '%TIKA_SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %TIKA_PATH%: download failed 1>&2
        del %TIKA_PATH% 2> NUL
        del %JAI_IMAGEIO_JPEG2000_PATH% 2> NUL
      )
    )
  )
)
java -jar %TIKA_PATH% %3 %4 %5 %6 %7 %8 %9 "%~1" > "%~2"
