@echo off
setlocal EnableDelayedExpansion
set PlantUMLVer=1.2023.0
set PlantUMLJar=plantuml-%PlantUMLVer%.jar
set DOWNLOAD_URL=https://github.com/plantuml/plantuml/releases/download/v%PlantUMLVer%/%PlantUMLJar%
set PlantUML_PATH=Commands\PlantUML\%PlantUMLJar%
set MESSAGE='PlantUML is not installed. Do you want to download it and its dependences from %DOWNLOAD_URL%'
set TITLE='PlantUML Plugin'
set PlantUML_SHA256=0404edcf0af28e5b409bc17aa59ad8b05051f47347377749c46c8018135d0dec

cd /d "%APPDATA%\WinMerge"
if not exist %PlantUML_PATH% (
  cd /d "%~dp0..\.."
  if not exist %PlantUML_PATH% (
    mkdir "%APPDATA%\WinMerge" 2> NUL
    cd /d "%APPDATA%\WinMerge"
    for %%i in (%PlantUML_PATH%) do mkdir %%~pi 2> NUL
    powershell "if ((New-Object -com WScript.Shell).Popup(%MESSAGE%,0,%TITLE%,1) -ne 1) { throw }" > NUL
    if errorlevel 1 (
      echo "download is canceled" 1>&2
    ) else (
      start "Downloading..." /WAIT powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %DOWNLOAD_URL% -Outfile %PlantUML_PATH%"
      powershell -command "$(CertUtil -hashfile '%PlantUML_PATH%' SHA256)[1] -replace ' ','' -eq '%PlantUML_SHA256%'" | findstr True > NUL
      if errorlevel 1 (
        echo %PlantUML_PATH%: download failed 1>&2
        del %PlantUML_PATH% 2> NUL
      )
    )
  )
)

(
  (
    (if "%~x1" == ".json" echo @startjson)
    (if "%~x1" == ".yml" echo @startyaml)
    (if "%~x1" == ".yaml" echo @startyaml)
  )
  type "%~1"
  (
    (if "%~x1" == ".json" echo @endjson)
    (if "%~x1" == ".yml" echo @endyaml)
    (if "%~x1" == ".yaml" echo @endyaml)
  )
) | call "%~dp0..\Java\java.bat" -jar "%CD%\%PlantUML_PATH%" -pipe %3 %4 %5 %6 %7 %8 %9 > "%~2"
