set exepath=c:\progra~1\winmerge\winmergeu.exe
for /F "usebackq" %%f in (`powershell -command [System.Diagnostics.FileVersionInfo]::GetVersionInfo^('%exepath%'^).FileVersion`) do (
  set resultpath=%~dp0\result-%%f-%date:/=-%%time::=.%.txt
)
(
  echo [MainFrame]
  powershell -command $^(measure-command { cmd '/c for /L %%i in ^(1,1,10^) do "%exepath%" /noninteractive /noprefs' }^).TotalSeconds
  echo [MergeEditFrame]
  powershell -command $^(measure-command { cmd '/c for /L %%i in ^(1,1,10^) do "%exepath%" /noninteractive /noprefs c:\progra~1\winmerge\docs\readme.txt c:\progra~1\winmerge\docs\readme.txt' }^).TotalSeconds
  echo [DirFrame]
  powershell -command $^(measure-command { cmd '/c for /L %%i in ^(1,1,10^) do "%exepath%" /noninteractive /noprefs c:\progra~1\winmerge\docs\ c:\progra~1\winmerge\docs\' }^).TotalSeconds
) > "%resultpath%"
