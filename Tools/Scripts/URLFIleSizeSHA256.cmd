pushd %~dp0
powershell -executionpolicy remotesigned -file %~dp0\URLFileSizeSHA256.ps1
popd