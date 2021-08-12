if "%1" == "" (
  set exepath=c:\progra~1\winmerge\winmergeu.exe
) else (
  set exepath=%1
)
if not exist %~dp0result.csv (
  echo Date,Version,MainFrame,MergeEditFrame,DirFrame,Report > "%~dp0result.csv"
)
for /F "usebackq" %%f in (`powershell -command [System.Diagnostics.FileVersionInfo]::GetVersionInfo^('%exepath%'^).FileVersion`) do set Version=%%f
for /F "usebackq" %%f in (`powershell -command $^(measure-command { cmd '/c for /L %%i in ^(1,1,10^) do "%exepath%" /noninteractive /noprefs' }^).TotalSeconds`) do set MainFrame=%%f
for /F "usebackq" %%f in (`powershell -command $^(measure-command { cmd '/c for /L %%i in ^(1,1,10^) do "%exepath%" /noninteractive /noprefs c:\progra~1\winmerge\docs\readme.txt c:\progra~1\winmerge\docs\readme.txt' }^).TotalSeconds`) do set MergeEditFrame=%%f
for /F "usebackq" %%f in (`powershell -command $^(measure-command { cmd '/c for /L %%i in ^(1,1,10^) do "%exepath%" /noninteractive /noprefs c:\progra~1\winmerge\docs\ c:\progra~1\winmerge\docs\' }^).TotalSeconds`) do set DirFrame=%%f
if "%Version:2.16=%" == "%Version%" goto :SKIP_REPORT
for /F "usebackq" %%f in (`powershell -command $^(measure-command { cmd '/c %~dp0\ReportTest.cmd' }^).TotalSeconds`) do set Report=%%f
:SKIP_REPORT
echo %date% %time%,%Version%,%MainFrame%,%MergeEditFrame%,%DirFrame%,%Report% >> "%~dp0result.csv"
