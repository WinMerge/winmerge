@echo off
setlocal EnableDelayedExpansion
set TikaVer=3.0.0
set TikaJar=tika-app-%TikaVer%.jar
set DOWNLOAD_URL=https://repo1.maven.org/maven2/org/apache/tika/tika-app/%TikaVer%/%TikaJar%
set TIKA_PATH=Commands\Apache-Tika\%TikaJar%
set MESSAGE='Apache Tika is not installed. Do you want to download it from %DOWNLOAD_URL%'
set TITLE='Apache Tika Plugin'
set TIKA_SHA256=f2c156533fac004d3d30d322555bb1f2581a104558a913bfc74d8c48dcf4541c

cd /d "%APPDATA%\WinMerge"
if not exist %TIKA_PATH% (
  cd /d "%~dp0..\.."
  if not exist %TIKA_PATH% (
    mkdir "%APPDATA%\WinMerge" 2> NUL
    cd /d "%APPDATA%\WinMerge"
    for %%i in (%TIKA_PATH%) do mkdir %%~pi 2> NUL
    powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
    if errorlevel 1 (
      echo "download is canceled" 1>&2
    ) else (
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri %DOWNLOAD_URL% -UseBasicParsing -Outfile %TIKA_PATH%"
      powershell -command "$(CertUtil -hashfile '%TIKA_PATH%' SHA256)[1] -replace ' ','' -eq '%TIKA_SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %TIKA_PATH%: download failed 1>&2
        del %TIKA_PATH% 2> NUL
      )
    )
  )
)
set TEMP_FILE=%TEMP%\tempfile_%RANDOM%%~x1
chcp 1252 >NUL
(echo "%~1") | findstr /C:"%~1" >NUL
if errorlevel 1 (
  if /i "%~1"=="%~s1" (
    mklink /h "%TEMP_FILE%" "%~1" >NUL 2>NUL
    if errorlevel 1 (
      copy "%~1" "%TEMP_FILE%" >NUL
    )
    call "%~dp0..\Java\java.bat" -jar "%CD%\%TIKA_PATH%" %3 %4 %5 %6 %7 %8 %9 "%TEMP_FILE%" > "%~2"
    del "%TEMP_FILE%" >NUL
  ) else (
    call "%~dp0..\Java\java.bat" -jar "%CD%\%TIKA_PATH%" %3 %4 %5 %6 %7 %8 %9 "%~s1" > "%~2"
  )
) else (
  call "%~dp0..\Java\java.bat" -jar "%CD%\%TIKA_PATH%" %3 %4 %5 %6 %7 %8 %9 "%~1" > "%~2"
)
