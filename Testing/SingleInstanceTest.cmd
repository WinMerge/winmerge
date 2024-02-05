setlocal enabledelayedexpansion
if not defined exepath (
  set exepath=%~dp0\..\Build\x64\Release\WinMergeU.exe
)
for /L %%i in (0,1,5) do ( 
  set cnt=%%i
  set /a lrev=cnt
  set /a rrev=cnt+1
  echo %exepath% -s:2 -u %%1 %%2> %~dp0\tmp.cmd
  git difftool --extcmd %~dp0\tmp.cmd HEAD~!lrev!..HEAD~!rrev! ../Src/MainFrm.cpp
)
rem del %~dp0\tmp.cmd
