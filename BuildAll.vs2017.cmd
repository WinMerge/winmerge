cd /d "%~dp0"
call BuildManual.cmd
call BuildBin.vs2017.cmd
call BuildInstaller.cmd
call BuildArc.cmd
