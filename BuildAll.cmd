pushd "%~dp0"
call BuildBin.cmd %1 %2 || goto :eof
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || goto :eof
popd
call BuildTranslations.cmd
call BuildInstaller.cmd %1 %2
call BuildArc.cmd %1 %2
popd
