pushd "%~dp0"
call BuildManual.cmd || goto :eof
call BuildBin.cmd %1 %2 || goto :eof
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || goto :eof
popd
call BuildInstaller.cmd %1 %2
call BuildArc.cmd %1 %2
popd
