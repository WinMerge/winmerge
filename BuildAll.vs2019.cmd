pushd "%~dp0"
call BuildManual.cmd
call BuildBin.vs2019.cmd %1 %2
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || goto :eof
popd
call BuildInstaller.cmd %1 %2
call BuildArc.cmd %1 %2
popd
