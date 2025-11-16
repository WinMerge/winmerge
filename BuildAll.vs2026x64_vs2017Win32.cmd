pushd "%~dp0"
call BuildManual.cmd || goto :eof
setlocal
call BuildBin.vs2017.cmd x86 || goto :eof
endlocal
setlocal
call BuildBin.vs2017.cmd ARM || goto :eof
endlocal
setlocal
call BuildBin.vs2026.cmd ARM64 || goto :eof
endlocal
setlocal
call BuildBin.vs2026.cmd x64 || goto :eof
endlocal
pushd Testing\GoogleTest\UnitTests
UnitTests.exe || goto :eof
popd
call BuildInstaller.cmd %1 %2
call BuildArc.cmd %1 %2
popd
