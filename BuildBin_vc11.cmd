cd /d "%~dp0"

del /s Build\*.exe
del /s BuildTmp\*.res

call SetVersion.cmd
cscript /nologo ExpandEnvironmenStrings.vbs Version.in > Version.h

setlocal
set VisualStudioVersion=11.0
call "%VS110COMNTOOLS%vsvars32.bat"
for %%i in ( ^
  ..\freeimage\Source\FreeImageLib\FreeImageLib.vcxproj ^
  ..\freeimage\Wrapper\FreeImagePlus\FreeImagePlus.vcxproj ^
  src\WinIMergeLib.vcxproj ^
  src\WinIMerge.vcxproj ^
  ) do (
  MSBuild %%i /t:build /p:Configuration=Release /p:Platform="Win32" /p:PlatformToolset=v110_xp || pause
  MSBuild %%i /t:build /p:Configuration=Release /p:Platform="x64" /p:PlatformToolset=v110_xp || pause
)
endlocal

