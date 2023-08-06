if "%1" == "" (
  set exepath=c:\program files\winmerge\winmergeu.exe
) else (
  set exepath=%1
)
for %%i in ("%exepath%") do set exedir=%%~dpi

if not exist %~dp0result.csv (
  echo Date,Version,MainFrame,MergeEditFrame,DirFrame,Report > "%~dp0result.csv"
)
for /F "usebackq" %%f in (`powershell -command [System.Diagnostics.FileVersionInfo]::GetVersionInfo^('%exepath%'^).FileVersion`) do set Version=%%f
for /F "usebackq" %%f in (`powershell -command $^(measure-command { for ^($i ^= 0^; $i -lt 10^; $i++^) { ^^^& '%exepath%' /noninteractive /noprefs ^^^| Out-Null } }^).TotalSeconds`) do set MainFrame=%%f
for /F "usebackq" %%f in (`powershell -command $^(measure-command { for ^($i ^= 0^; $i -lt 10^; $i++^) { ^^^& '%exepath%' /noninteractive /noprefs '%exedir%docs\readme.txt' '%exedir%docs\readme.txt' ^^^| Out-Null } }^).TotalSeconds`) do set MergeEditFrame=%%f
for /F "usebackq" %%f in (`powershell -command $^(measure-command { for ^($i ^= 0^; $i -lt 10^; $i++^) { ^^^& '%exepath%' /noninteractive /noprefs '%exedir%docs\' '%exedir%docs\' ^^^| Out-Null } }^).TotalSeconds`) do set DirFrame=%%f
if "%Version:2.16=%" == "%Version%" goto :SKIP_REPORT
for /F "usebackq" %%f in (`powershell -command $^(measure-command { cmd '/c %~dp0\ReportTest.cmd' }^).TotalSeconds`) do set Report=%%f
:SKIP_REPORT
echo %date% %time%,%Version%,%MainFrame%,%MergeEditFrame%,%DirFrame%,%Report% >> "%~dp0result.csv"
