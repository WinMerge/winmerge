pushd "%~dp0"
call BuildManual.cmd
call BuildBin.vs2017.cmd
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || exit
popd
call BuildInstaller.cmd
call BuildArc.cmd
popd
