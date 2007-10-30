@goto _MSDEV_BLD_ENV_(%_MSDEV_BLD_ENV_%)
:_MSDEV_BLD_ENV_()
@echo Merge.dsp/Merge.vcproj post-build script
@echo Not intended for direct invocation through user interface
@echo Post-build command line: PostBuild.bat $(IntDir) $(TargetPath)
@pause
@exit

:_MSDEV_BLD_ENV_(1)
cd
echo %0
echo $(IntDir) = %1
echo $(TargetPath) = %2
REM Create English.pot and MergeLang.rc from Merge.rc
cd Languages
cscript CreateMasterPotFile.vbs
cd ..
REM Create MergeLang.dll from MergeLang.rc
rc /fo%1\MergeLang.res /i.. Languages\MergeLang.rc
link /DLL /NOENTRY /MACHINE:IX86 /OUT:%2\..\MergeLang.dll %1\MergeLang.res
REM Copy PO files to where WinMerge expects them
mkdir %2\..\Languages
copy Languages\*.po %2\..\Languages
