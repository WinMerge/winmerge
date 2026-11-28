pushd %~dp0
powershell -executionpolicy remotesigned -file %~dp0\UpdateDownloadInfo.ps1 %*
popd