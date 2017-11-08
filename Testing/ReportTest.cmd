setlocal enabledelayedexpansion
set cdate=%date:~0,10%
set cdate=%cdate:/=-%
set ctime=%time::=%
set ctime=%ctime: =%
set outputdir=%~dp0\..\BuildTmp\ReportTest\%cdate%-%ctime%
echo %outputdir%
mkdir %outputdir%
for /L %%i in (989,1,999) do ( 
  set cnt=%%i
  set /a lrev=cnt
  set /a rrev=cnt+1
  hg --config extensions.extdiff= --config extdiff.cmd.winmerge=%~dp0\..\Build\x64\MergeUnicodeDebug\WinMergeU.exe --config extdiff.opts.winmerge="-minimize -noninteractive -noprefs -cfg Settings/DirViewExpandSubdirs=1 -cfg ReportFiles/ReportType=2 -cfg ReportFiles/IncludeFileCmpReport=1 -r -u -or %outputdir%\%%i.html" winmerge  -r !lrev! -r !rrev!
)
