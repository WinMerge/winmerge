@echo off
echo ^<!DOCTYPE html^>
echo ^<html^>
echo ^<head^>
echo ^<title^>^</title^>
echo ^<meta name="generator" content="md2html"^>
echo ^<base href="file:///%~dp1"^>
echo ^</head^>
echo ^<body^>
type %1 | "%~dp0\md2html.exe" %2 %3 %4 %5 %6 %7 %8 %9
echo ^</body^>
echo ^</html^>
