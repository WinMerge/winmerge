@echo off
setlocal EnableDelayedExpansion
set TikaVer=2.1.0
set JaiVer=1.4.0
set TikaJar=tika-app-%TikaVer%.jar
set JaiJar=jai-imageio-jpeg2000-%JaiVer%.jar
set DOWNLOAD_URL=https://repo1.maven.org/maven2/org/apache/tika/tika-app/%TikaVer%/%TikaJar%
set DOWNLOAD_URL_JPEG2000=https://repo1.maven.org/maven2/com/github/jai-imageio/jai-imageio-jpeg2000/%JaiVer%/%JaiJar%
set TIKA_PATH=Commands\Apache-Tika\%TikaJar%
set JAI_IMAGEIO_JPEG2000_PATH=Commands\Apache-Tika\%JaiJar%
set MESSAGE='Apache Tika is not installed. Do you want to download it and its dependences from %DOWNLOAD_URL% and %DOWNLOAD_URL_JPEG2000%?'
set TITLE='Apache Tika Plugin'
set TIKA_SHA256=0a93cdffebe1f1f0aca5b203538cafd66579a65409a8d565b93d3b8150e4e69c
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
