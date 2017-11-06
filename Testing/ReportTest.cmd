setlocal enabledelayedexpansion
set cdate=%date:~0,10%
set cdate=%cdate:/=-%
set ctime=%time::=%
set ctime=%ctime: =%
set outputdir=%~dp0\..\BuildTmp\ReportTest\%cdate%-%ctime%
echo %outputdir%
mkdir %outputdir%
for /L %%i in (1,1,10) do ( 
  set cnt=%%i
  set /a lrev=999-!cnt!
  set /a rrev=1000-!cnt!
  hg --config extensions.extdiff= --config extdiff.cmd.winmerge=%~dp0\..\Build\x64\MergeUnicodeDebug\WinMergeU.exe --config extdiff.opts.winmerge="-minimize -noninteractive -noprefs -cfg Settings/DirViewExpandSubdirs=1 -r -u -or %outputdir%\%%i.html" winmerge  -r !lrev! -r !rrev!
)
