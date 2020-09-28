powershell -command measure-command { cmd '/c for /L %%i in (1,1,10) do c:\progra~1\winmerge\winmergeu.exe /noninteractive /noprefs' } > result.txt
powershell -command measure-command { cmd '/c for /L %%i in (1,1,10) do c:\progra~1\winmerge\winmergeu.exe /noninteractive /noprefs c:\progra~1\winmerge\docs\readme.txt c:\progra~1\winmerge\docs\readme.txt' } >> result.txt
powershell -command measure-command { cmd '/c for /L %%i in (1,1,10) do c:\progra~1\winmerge\winmergeu.exe /noninteractive /noprefs c:\progra~1\winmerge\docs\ c:\progra~1\winmerge\docs\' } >> result.txt
