pushd "%~dp0"
call BuildManual.cmd
setlocal
call BuildBin.vs2017.cmd Win32
endlocal
setlocal
call BuildBin.vs2019.cmd x64
endlocal
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || exit
popd
call BuildInstaller.cmd %1 %2
call BuildArc.cmd %1 %2
popd
