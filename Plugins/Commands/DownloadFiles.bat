@echo off
for %%i in (Apache-Tika Java PlantUML q yq) do (
  for /F %%j in ('type %%i\URL.txt') do (
    echo Downloading %%j
    mkdir "%APPDATA%\WinMerge\Commands\%%i" 2> NUL
    powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri %%j -UseBasicParsing -Outfile '%APPDATA%\WinMerge\Commands\%%i\%%~nxj'"
  )
)
