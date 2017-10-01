cd /d "%~dp0"
call BuildManual.cmd
call BuildBin.vs2015.cmd
call BuildInstaller.cmd
call BuildArc.cmd
