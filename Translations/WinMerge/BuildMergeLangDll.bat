cd
echo %0
echo $(IntDir) = %1
echo $(TargetPath) = %2
cd Languages
cscript CreateMasterPotFile.vbs
cd ..
rc /fo%1\MergeLang.res /i.. Languages\MergeLang.rc
link /DLL /NOENTRY /MACHINE:IX86 /OUT:%2\..\MergeLang.dll %1\MergeLang.res
