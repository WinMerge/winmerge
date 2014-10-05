cd /d "%~dp0"

del /s Build\*.exe
del /s BuildTmp\*.res

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

setlocal
call "%VS100COMNTOOLS%vsvars32.bat"
for %%i in ( ^
  ..\freeimage\Source\FreeImageLib\FreeImageLib.vcxproj ^
  ..\freeimage\Wrapper\FreeImagePlus\FreeImagePlus.vcxproj ^
  src\WinIMergeLib.vcxproj ^
  src\WinIMerge.vcxproj ^
  src\cidiff.vcxproj ^
  ) do (
  MSBuild %%i /t:build /p:Configuration=Release /p:Platform="Win32" || pause
  MSBuild %%i /t:build /p:Configuration=Release /p:Platform="x64" || pause
)
endlocal

