@echo off
setlocal EnableDelayedExpansion
set /a "ii=%RANDOM%*4/32678"
set DOWNLOAD_URL0=https://ftp.jaist.ac.jp/pub/apache/tika/2.0.0/tika-app-2.0.0.jar
set DOWNLOAD_URL1=https://ftp.riken.jp/net/apache/tika/2.0.0/tika-app-2.0.0.jar
set DOWNLOAD_URL2=https://ftp.tsukuba.wide.ad.jp/software/apache/tika/2.0.0/tika-app-2.0.0.jar
set DOWNLOAD_URL3=https://ftp.yz.yamagata-u.ac.jp/pub/network/apache/tika/2.0.0/tika-app-2.0.0.jar
set DOWNLOAD_URL=!DOWNLOAD_URL%ii%!
set DOWNLOAD_URL_JPEG2000=https://github.com/jai-imageio/jai-imageio-jpeg2000/releases/download/jai-imageio-jpeg2000-1.4.0/jai-imageio-jpeg2000-1.4.0.jar
set TIKA_PATH=Commands\Apache-Tika\tika-app-2.0.0.jar
set JAI_IMAGEIO_JPEG2000_PATH=Commands\Apache-Tika\jai-imageio-jpeg2000-1.4.0.jar
set MESSAGE='Apache Tika is not installed. Do you want to download it and its dependences from %DOWNLOAD_URL% and %DOWNLOAD_URL_JPEG2000%?'
set TITLE='Apache Tika Plugin'
set TIKA_SHA256=465d0a157c62ffbbdb02a3d340b93c78d03d344e8f046fe9e6033573e9cf4b8c
set JPEG2000_SHA256=07fb6e3a3040122b846c5e52520033175c3251e2ec8830df82f87cb21f388bb1


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
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL_JPEG2000% -Outfile %JAI_IMAGEIO_JPEG2000_PATH%"
      powershell -command "$(CertUtil -hashfile '%TIKA_PATH%' SHA256)[1] -replace ' ','' -eq '%TIKA_SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %TIKA_PATH%: download failed 1>&2
        del %TIKA_PATH% 2> NUL
        del %JAI_IMAGEIO_JPEG2000_PATH% 2> NUL
      )
      powershell -command "$(CertUtil -hashfile %JAI_IMAGEIO_JPEG2000_PATH% SHA256)[1] -replace ' ','' -eq '%JPEG2000_SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %JAI_IMAGEIO_JPEG2000_PATH%: download failed 1>&2
        del %TIKA_PATH% 2> NUL
        del %JAI_IMAGEIO_JPEG2000_PATH% 2> NUL
      )
    )
  )
)
java -Xbootclasspath/a:%JAI_IMAGEIO_JPEG2000_PATH% -jar %TIKA_PATH% %3 %4 %5 %6 %7 %8 %9 "%~1" > "%~2"
