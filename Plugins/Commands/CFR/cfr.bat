@echo off
setlocal EnableDelayedExpansion
set CFRVer=0.152
set CFRJar=cfr-%CFRVer%.jar
set DOWNLOAD_URL=https://github.com/leibnitz27/cfr/releases/download/%CFRVer%/%CFRJar%
set CFR_PATH=Commands\CFR\%CFRJar%
set MESSAGE='CFR Java Decompiler is not installed. Do you want to download it from %DOWNLOAD_URL%'
set TITLE='CFR Java Decompiler Plugin'
set CFR_SHA256=f686e8f3ded377d7bc87d216a90e9e9512df4156e75b06c655a16648ae8765b2

cd /d "%APPDATA%\WinMerge"
if not exist %CFR_PATH% (
  cd /d "%~dp0..\.."
  if not exist %CFR_PATH% (
    mkdir "%APPDATA%\WinMerge" 2> NUL
    cd /d "%APPDATA%\WinMerge"
    for %%i in (%CFR_PATH%) do mkdir %%~pi 2> NUL
    powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
    if errorlevel 1 (
      echo "download is canceled" 1>&2
    ) else (
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri %DOWNLOAD_URL% -UseBasicParsing -Outfile %CFR_PATH%"
      powershell -command "$(CertUtil -hashfile %CFR_PATH% SHA256)[1] -replace ' ','' -eq '%CFR_SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %CFR_PATH%: download failed 1>&2
        del %CFR_PATH% 2> NUL
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
    call "%~dp0..\Java\java.bat" -jar "%CD%\%CFR_PATH%" %3 %4 %5 %6 %7 %8 %9 "%TEMP_FILE%" > "%~2"
    del "%TEMP_FILE%" >NUL
  ) else (
    call "%~dp0..\Java\java.bat" -jar "%CD%\%CFR_PATH%" %3 %4 %5 %6 %7 %8 %9 "%~s1" > "%~2"
  )
) else (
  call "%~dp0..\Java\java.bat" -jar "%CD%\%CFR_PATH%" %3 %4 %5 %6 %7 %8 %9 "%~1" > "%~2"
)
