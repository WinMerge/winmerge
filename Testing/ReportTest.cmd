setlocal enabledelayedexpansion
if not defined exepath (
  set exepath=%~dp0\..\Build\x64\Release\WinMergeU.exe
)
set cdate=%date:~0,10%
set cdate=%cdate:/=-%
set ctime=%time::=%
set ctime=%ctime: =%
set outputdir=%~dp0\..\BuildTmp\ReportTest\%cdate%-%ctime%
echo %outputdir%
mkdir %outputdir%
for /L %%i in (0,1,5) do ( 
  set cnt=%%i
  set /a lrev=cnt
  set /a rrev=cnt+1
  echo "%exepath%" -minimize -noninteractive -noprefs -cfg Settings/DirViewExpandSubdirs=1 -cfg ReportFiles/ReportType=2 -cfg ReportFiles/IncludeFileCmpReport=1 -r -u -or %outputdir%\%%i.html %%1 %%2> %~dp0\tmp.cmd
  git difftool -d --extcmd %~dp0\tmp.cmd R2_10_0~!lrev!..R2_10_0~!rrev!
)
del %~dp0\tmp.cmd
