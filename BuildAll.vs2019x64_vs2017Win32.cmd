pushd "%~dp0"
call BuildManual.cmd || goto :eof
setlocal
call BuildBin.vs2017.cmd Win32 || goto :eof
endlocal
setlocal
call BuildBin.vs2019.cmd ARM64 || goto :eof
endlocal
setlocal
call BuildBin.vs2019.cmd x64 || goto :eof
endlocal
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || goto :eof
popd
call BuildInstaller.cmd %1 %2
call BuildArc.cmd %1 %2
popd
