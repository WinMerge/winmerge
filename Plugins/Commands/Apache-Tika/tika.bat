@echo off
setlocal EnableDelayedExpansion
set /a "ii=%RANDOM%*4/32678"
set DOWNLOAD_URL0=https://ftp.jaist.ac.jp/pub/apache/tika/2.0.0-BETA/tika-app-2.0.0-BETA.jar
set DOWNLOAD_URL1=https://ftp.riken.jp/net/apache/tika/2.0.0-BETA/tika-app-2.0.0-BETA.jar
set DOWNLOAD_URL2=https://ftp.tsukuba.wide.ad.jp/software/apache/tika/2.0.0-BETA/tika-app-2.0.0-BETA.jar
set DOWNLOAD_URL3=https://ftp.yz.yamagata-u.ac.jp/pub/network/apache/tika/2.0.0-BETA/tika-app-2.0.0-BETA.jar
set DOWNLOAD_URL=!DOWNLOAD_URL%ii%!
set DOWNLOAD_URL_JPEG2000=https://github.com/jai-imageio/jai-imageio-jpeg2000/releases/download/jai-imageio-jpeg2000-1.4.0/jai-imageio-jpeg2000-1.4.0.jar
set TIKA_PATH=WinMerge\Commands\Apache-Tika\tika-app-2.0.0-BETA.jar
set JAI_IMAGEIO_JPEG2000_PATH=WinMerge\Commands\Apache-Tika\jai-imageio-jpeg2000-1.4.0.jar
set MESSAGE='Apache Tika is not installed. Do you want to download it and its dependences from %DOWNLOAD_URL% and %DOWNLOAD_URL_JPEG2000%?'
set TITLE='Apache Tika Plugin'

cd "%APPDATA%"
if not exist %TIKA_PATH% (
  for %%i in (%TIKA_PATH%) do mkdir %%~pi 2> NUL
  powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
  if errorlevel 1 (
    echo "download is canceled" 1>&2
  ) else (
    start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL% -Outfile %TIKA_PATH%"
    start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL_JPEG2000% -Outfile %JAI_IMAGEIO_JPEG2000_PATH%"
  )
)
java -Xbootclasspath/a:%JAI_IMAGEIO_JPEG2000_PATH% -jar %TIKA_PATH% %3 %4 %5 %6 %7 %8 %9 "%~1" > "%~2"
